#include <IconicMeasureCommon/MeasureHandler.h>
#include <IconicSensor/Camera.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/ffile.h>
#include <wx/wx.h>


using namespace iconic;

MeasureHandler::MeasureHandler() : cbIsParsed(false) {
	cpSelectedShape = NULL;
}
MeasureHandler::~MeasureHandler()  
{
	for (ShapePtr s : cvShapes) delete s.get();
	cpSelectedShape = NULL;

}

bool MeasureHandler::OnNextFrame(gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse) {
	cpProperties = pProperties;
	cImageFileName = filename;
	wxFileName fn(cImageFileName);
	fn.SetExt("dmp");
	cDepthMapFileName = fn.GetFullPath();
	fn.SetExt("cam");
	cCameraFileName = fn.GetFullPath();
	cbIsParsed = false;
	if (bDoParse) {
		if (!Parse()) {
			wxLogError("Could not parse meta data");
			return false;
		}
	}
	return true;
}

wxString MeasureHandler::GetMetaFileName() const {
	return cDepthMapFileName;
}

bool MeasureHandler::Parse() {
	if (cbIsParsed) {
		return true;
	}
	if (!ReadDepthMap()) {
		wxLogError(_("Could not read depth map"));
		return false;
	}

	if (!ReadCamera()) {
		wxLogError(_("Could not read camera"));
		return false;
	}

	return true;
}

bool MeasureHandler::ReadDepthMap() {
	if (!wxFileName::FileExists(cDepthMapFileName)) {
		wxLogError("Depth map is missing (%s)", cDepthMapFileName);
		return false;
	}

	if (!cpProperties) {
		wxLogError(_("Could not get image properties"));
		return false;
	}
	cpProperties->GetImageSize(cGeometry.cImageSize[0], cGeometry.cImageSize[1]);

	const size_t nPixels = cGeometry.cImageSize[0] * cGeometry.cImageSize[1];
	if (cGeometry.cDepthMap.size() != nPixels) {
		cGeometry.cDepthMap.resize(nPixels);
	}
	wxFFile file(cDepthMapFileName, "rb");
	if (!file.IsOpened()) {
		wxLogError(_("Could not open %s"), cDepthMapFileName);
		return false;
	}
	if (file.Read(cGeometry.cDepthMap.data(), sizeof(float) * nPixels) != sizeof(float) * nPixels) {
		wxLogError(_("Not enough data in %s"), cDepthMapFileName);
		return false;
	}
	return true;
}

bool MeasureHandler::ReadCamera() {
	if (!wxFileName::FileExists(cCameraFileName)) {
		wxLogError("Camera file is missing (%s)", cCameraFileName);
		return false;
	}
	wxFFile file(cCameraFileName, "rb");
	if (!file.IsOpened()) {
		wxLogError(_("Could not read camera file %s"), cCameraFileName);
		return false;
	}

	// The written camera is a 3x4 matrix with doubles, which is how the GpuCamera is defined
	GpuCamera gpuCamera;
	if (file.Read(&gpuCamera, sizeof(GpuCamera)) != sizeof(GpuCamera)) {
		wxLogError(_("Not enough data in %s"), cCameraFileName);
		return false;
	}

	// A more useful camera class is the Camera pointed to by CameraPtr so we copy the read GpuCamera to a Camera
	CameraPtr pCamera = GetCamera();
	*pCamera = gpuCamera;

	Camera::Camera2PixelMatrix(cGeometry.cImageSize[0], cGeometry.cImageSize[1], cGeometry.cCameraToPixelTransform);

	CheckCamera();

	cbIsParsed = true;
	return true;
}

void MeasureHandler::CheckCamera() {
	if (!cGeometry.cpCamera) {
		cGeometry.cCameraType = Camera::ECameraType::FULL;
		return;
	}
	cGeometry.cCameraType = cGeometry.cpCamera->ClassifyCamera();
}

bool MeasureHandler::ImageToObject(iconic::Geometry::PolygonPtr pImage, iconic::Geometry::Polygon3DPtr pObject) {
	return cGeometry.ImageToObject(pImage, pObject);
}

bool MeasureHandler::ImageToObject(const iconic::Geometry::Point& imagePt, iconic::Geometry::Point3D& objectPt) {
	return cGeometry.ImageToObject(imagePt, objectPt);
}

bool MeasureHandler::ImageToObject(const std::vector<iconic::Geometry::Point>& vImageVector, std::vector<iconic::Geometry::Point3D>& vObjectVector) {
	return cGeometry.ImageToObject(vImageVector, vObjectVector);
}

CameraPtr MeasureHandler::GetCamera() {
	if (!cGeometry.cpCamera) {
		cGeometry.cpCamera = boost::make_shared<Camera>();
	}
	return cGeometry.cpCamera;
}

std::vector<float>& MeasureHandler::GetDepthMap() {
	return cGeometry.cDepthMap;
}

void MeasureHandler::GetImageSize(size_t& width, size_t& height) {
	width = cGeometry.cImageSize[0];
	height = cGeometry.cImageSize[1];
}

bool MeasureHandler::InstantiateNewShape(iconic::ShapeType type) {
	if (cpSelectedShape) {
		wxLogVerbose(_("Cannot instantiate new shape since one is already selected"));
		return false;
	}

	static int c = 0;
	wxColor col = cGeometry.GetColour((Geometry::Colours)(c % 6));
	c = (c + 1) % 6;
	switch (type) {
	case iconic::ShapeType::PointType:
		cpSelectedShape = iconic::ShapePtr(new iconic::PointShape(col));
		break;
	case iconic::ShapeType::LineType:
		cpSelectedShape = iconic::ShapePtr(new iconic::LineShape(col));
		break;
	case iconic::ShapeType::PolygonType:
		cpSelectedShape = iconic::ShapePtr(new iconic::PolygonShape(col));
		break;
	}

	cvShapes.push_back(cpSelectedShape);
	cSelectedShapeIndex = cvShapes.size() - 1;
	wxLogVerbose(_("There are currently " + std::to_string(cvShapes.size()) + " number of shapes"));
	return true;
}

void MeasureHandler::AddImagePolygon(iconic::Geometry::PolygonPtr pPolygon) {
	static int c = 0;
	wxColor col = cGeometry.GetColour((Geometry::Colours)c);
	c = (c + 1) % 6;

	cpSelectedShape = iconic::ShapePtr(new iconic::PolygonShape(pPolygon, col));
	cvShapes.push_back(cpSelectedShape);
	cSelectedShapeIndex = cvShapes.size() - 1;
}

bool MeasureHandler::ModifySelectedShape(Geometry::Point imgP, MeasureEvent::EAction modification, DataUpdateEvent& e) {
	if (!cpSelectedShape) {
		return false; // No shape to add point to
	}
	bool r = false;
	switch (modification) {
	case MeasureEvent::EAction::SELECTED:
		cpSelectedShape->AddPoint(imgP, -1);
		// Invalidate data presentation of shape
		break;
	case MeasureEvent::EAction::ADDED:
		cpSelectedShape->DeselectPoint();

		if (cpSelectedShape->IsCompleted()) {
			cpSelectedShape->UpdateCalculations(cGeometry);

			e.Initialize(cSelectedShapeIndex, cpSelectedShape);
			if (cpSelectedShape->GetType() == iconic::ShapeType::PointType)
				HandleFinishedMeasurement();

			r = true;
		}

		break;
	case MeasureEvent::EAction::MOVED:
		if (cpSelectedShape) {
			cpSelectedShape->MoveSelectedPoint(imgP);
		}
		break;
	}
	return r;
}

void MeasureHandler::HandleFinishedMeasurement(bool instantiate_new) {
	if (!cpSelectedShape) {
		return;
	}

	cpSelectedShape->UpdateCalculations(cGeometry);

	iconic::ShapeType previousShapeType = cpSelectedShape->GetType();

	DeleteSelectedShapeIfIncomplete();

	cpSelectedShape = NULL;
	cSelectedShapeIndex = -1;


	if (instantiate_new) {
		InstantiateNewShape(previousShapeType);
	}
}

//Risky to just "pop_back", might go wrong in possible edge cases
void MeasureHandler::DeleteSelectedShapeIfIncomplete() {

	if (!cpSelectedShape->IsCompleted()) cvShapes.pop_back();

	cpSelectedShape = NULL;
	wxLogVerbose(_("There are currently " + std::to_string(cvShapes.size()) + " number of shapes"));
}

void MeasureHandler::ClearShapes() {
	cpSelectedShape = NULL;
	cvShapes.clear();
}

ShapeType MeasureHandler::SelectShapeFromCoordinates(Geometry::Point point) {
	// Loop over the the currently existing shapes
	for (int i = 0; i < cvShapes.size(); i++) {
		if (cvShapes[i]->Select(point)) {
			cpSelectedShape = cvShapes[i];
			cSelectedShapeIndex = i;
			return cpSelectedShape->GetType();
		}
	}
	// If no shape is clicked then make sure no shape is selected
	cpSelectedShape = NULL;
	cSelectedShapeIndex = -1;
	return ShapeType::None;
}

int MeasureHandler::DeleteSelectedShape() {
	if (cpSelectedShape == NULL
		|| cSelectedShapeIndex < 0
		|| cSelectedShapeIndex >= cvShapes.size()
	) {
		return -1;
	}
	int index = cSelectedShapeIndex;
	cvShapes.erase(cvShapes.begin() + cSelectedShapeIndex);
	cpSelectedShape = NULL;
	cSelectedShapeIndex = -1;
	return index;
}

bool MeasureHandler::GetWKT(std::string& wkt) {
	wkt.clear();
	const std::string srid = "SRID = 4326;";
	std::string s;
	bool temp, overall = false;
	for (ShapePtr shape : cvShapes) {
		temp = shape->GetWKT(s);
		if (!temp) continue;
		overall = true;
		wkt.append(srid);
		wkt.append(s);
		wkt.append(std::string("\n"));
		s.clear();
	}
	return overall;
}

bool MeasureHandler::LoadWKT(wxString& wkt, DataUpdateEvent& e) {
	if (wkt.empty()) return false;

	int start = wkt.find(';');
	if (start == wxNOT_FOUND) start = 0;
	else start++;

	static int c = 0;
	wxColor col = cGeometry.GetColour((Geometry::Colours)(c % 6));
	c = (c + 1) % 6;

	ShapePtr shape;
	if (wkt.Contains(_("POLYGON"))) {
		shape = iconic::ShapePtr(new iconic::PolygonShape(col, wkt.SubString(start, wkt.Length())));
	} else if (wkt.Contains(_("LINESTRING"))) {
		shape = iconic::ShapePtr(new iconic::LineShape(col, wkt.SubString(start, wkt.Length())));
	} else if(wkt.Contains(_("POINT"))) {
		shape = iconic::ShapePtr(new iconic::PointShape(col, wkt.SubString(start, wkt.Length())));
	}
	else {
		wxLogWarning(_("Incorrect line in WKT file: " + wkt));
		return false;
	}
	shape->UpdateCalculations(cGeometry);
	e.Initialize(cvShapes.size(), shape);

	cvShapes.push_back(shape);

	wxLogVerbose(_("There are currently " + std::to_string(cvShapes.size()) + " number of shapes"));
	return true;
}

void MeasureHandler::OnDrawShapes(DrawEvent& e) {
	for (const ShapePtr shape : cvShapes) {
		shape->Draw();
	}

	if (cpSelectedShape && cpSelectedShape->GetNumberOfPoints() > 0) { // Check for null values
		boost::compute::float2_ mousePos;
		e.GetPoint(mousePos.x, mousePos.y);
		Geometry::Point mouse(mousePos.x, mousePos.y);

		cpSelectedShape->Draw(true, e.IsMeasuring(), mouse);
	}
}
