#include <IconicMeasureCommon/MeasureHandler.h>
#include <IconicSensor/Camera.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/ffile.h>
#include <wx/wx.h>

using namespace iconic;

MeasureHandler::MeasureHandler() : cbIsParsed(false)
{
	selectedShape = NULL;
}

void MeasureHandler::SetSidePanelPtr(SidePanel* ptr) {
	sidePanel = ptr;
	sidePanel->SetShapesPtr(&shapes);
}

bool MeasureHandler::OnNextFrame(gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse)
{
	cpProperties = pProperties;
	cImageFileName = filename;
	wxFileName fn(cImageFileName);
	fn.SetExt("dmp");
	cDepthMapFileName = fn.GetFullPath();
	fn.SetExt("cam");
	cCameraFileName = fn.GetFullPath();
	cbIsParsed = false;
	if (bDoParse)
	{
		if (!Parse())
		{
			wxLogError("Could not parse meta data");
			return false;
		}
	}
	return true;
}

wxString MeasureHandler::GetMetaFileName() const
{
	return cDepthMapFileName;
}

bool MeasureHandler::Parse()
{
	if (cbIsParsed)
	{
		return true;
	}
	if (!ReadDepthMap())
	{
		wxLogError(_("Could not read depth map"));
		return false;
	}

	if (!ReadCamera())
	{
		wxLogError(_("Could not read camera"));
		return false;
	}

	return true;
}

bool MeasureHandler::ReadDepthMap()
{
	if (!wxFileName::FileExists(cDepthMapFileName))
	{
		wxLogError("Depth map is missing (%s)", cDepthMapFileName);
		return false;
	}

	if (!cpProperties)
	{
		wxLogError(_("Could not get image properties"));
		return false;
	}
	cpProperties->GetImageSize(cGeometry.cImageSize[0], cGeometry.cImageSize[1]);

	const size_t nPixels = cGeometry.cImageSize[0] * cGeometry.cImageSize[1];
	if (cGeometry.cDepthMap.size() != nPixels)
	{
		cGeometry.cDepthMap.resize(nPixels);
	}
	wxFFile file(cDepthMapFileName, "rb");
	if (!file.IsOpened())
	{
		wxLogError(_("Could not open %s"), cDepthMapFileName);
		return false;
	}
	if (file.Read(cGeometry.cDepthMap.data(), sizeof(float) * nPixels) != sizeof(float) * nPixels)
	{
		wxLogError(_("Not enough data in %s"), cDepthMapFileName);
		return false;
	}
	return true;
}

bool MeasureHandler::ReadCamera()
{
	if (!wxFileName::FileExists(cCameraFileName))
	{
		wxLogError("Camera file is missing (%s)", cCameraFileName);
		return false;
	}
	wxFFile file(cCameraFileName, "rb");
	if (!file.IsOpened())
	{
		wxLogError(_("Could not read camera file %s"), cCameraFileName);
		return false;
	}

	// The written camera is a 3x4 matrix with doubles, which is how the GpuCamera is defined
	GpuCamera gpuCamera;
	if (file.Read(&gpuCamera, sizeof(GpuCamera)) != sizeof(GpuCamera))
	{
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

void MeasureHandler::CheckCamera()
{
	if (!cGeometry.cpCamera)
	{
		cGeometry.cCameraType = Camera::ECameraType::FULL;
		return;
	}
	cGeometry.cCameraType = cGeometry.cpCamera->ClassifyCamera();
}

bool MeasureHandler::ImageToObject(iconic::Geometry::PolygonPtr pImage, iconic::Geometry::Polygon3DPtr pObject)
{
	return cGeometry.ImageToObject(pImage, pObject);
}

bool MeasureHandler::ImageToObject(const iconic::Geometry::Point& imagePt, iconic::Geometry::Point3D& objectPt)
{
	return cGeometry.ImageToObject(imagePt, objectPt);
}

bool MeasureHandler::ImageToObject(const std::vector<iconic::Geometry::Point>& vImageVector, std::vector<iconic::Geometry::Point3D>& vObjectVector)
{
	return cGeometry.ImageToObject(vImageVector, vObjectVector);
}

CameraPtr MeasureHandler::GetCamera()
{
	if (!cGeometry.cpCamera)
	{
		cGeometry.cpCamera = boost::make_shared<Camera>();
	}
	return cGeometry.cpCamera;
}

std::vector<float>& MeasureHandler::GetDepthMap()
{
	return cGeometry.cDepthMap;
}

void MeasureHandler::GetImageSize(size_t& width, size_t& height)
{
	width = cGeometry.cImageSize[0];
	height = cGeometry.cImageSize[1];
}

bool MeasureHandler::InstantiateNewShape(iconic::Geometry::ShapeType type) {
	if (this->selectedShape) {
		wxLogVerbose(_("Cannot instantiate new shape since one is already selected"));
		return false;
	}
	int col1 = rand();
	int col2 = rand();

	switch (type) {
	case iconic::Geometry::ShapeType::PointType:
		this->selectedShape = boost::shared_ptr<iconic::Geometry::Shape>((iconic::Geometry::Shape*)new iconic::Geometry::PointShape(iconic::Geometry::Color{ (unsigned char)(col1 >> 8), (unsigned char)col1, (unsigned char)col2, 150 }));
		break;
	case iconic::Geometry::ShapeType::LineType:
		this->selectedShape = boost::shared_ptr<iconic::Geometry::Shape>((iconic::Geometry::Shape*)new iconic::Geometry::LineShape(iconic::Geometry::Color{ (unsigned char)(col1 >> 8), (unsigned char)col1, (unsigned char)col2, 150 }));
		break;
	case iconic::Geometry::ShapeType::PolygonType:
		this->selectedShape = boost::shared_ptr<iconic::Geometry::Shape>((iconic::Geometry::Shape*)new iconic::Geometry::PolygonShape(iconic::Geometry::Color{ (unsigned char)(col1 >> 8), (unsigned char)col1, (unsigned char)col2, 150 }));
		break;
	}


	this->shapes.push_back(this->selectedShape);
	wxLogVerbose(_("There are currently " + std::to_string(this->shapes.size()) + " number of shapes"));
	return true;
}

bool MeasureHandler::AddPointToSelectedShape(iconic::Geometry::Point3D p, Geometry::Point imgP) {
	if (!this->selectedShape) {
		return false; // No shape to add point to
	}
	//this->selectedShape->dataPointer.get()->outer().push_back(p);
	this->selectedShape->AddPoint(imgP, -1);
	//this->selectedShape->renderCoordinates->outer().push_back(imgP);
	
	wxLogVerbose(_("There are currently " + std::to_string(this->selectedShape->GetNumberOfPoints()) + " number of renderpoints in this shape"));

	if (this->selectedShape->GetType() == iconic::Geometry::ShapeType::PointType) {
		HandleFinishedMeasurement();
	}

	return true; // Temporary solution
}

void MeasureHandler::HandleFinishedMeasurement(bool instantiate_new) {
	if (!this->selectedShape) {
		return;
	}

	this->selectedShape->UpdateCalculations(cGeometry);

	iconic::Geometry::ShapeType previousShapeType = this->selectedShape->GetType();
	this->DeleteSelectedShapeIfIncomplete();
	this->selectedShape = NULL;

	sidePanel->Update();

	if (instantiate_new) {
		this->InstantiateNewShape(previousShapeType);
	}
}

//Risky to just "pop_back", might go wrong in possible edge cases
void MeasureHandler::DeleteSelectedShapeIfIncomplete() {

	if (!this->selectedShape->IsCompleted()) shapes.pop_back();

	selectedShape = NULL;
	wxLogVerbose(_("There are currently " + std::to_string(this->shapes.size()) + " number of shapes"));
}

bool MeasureHandler::SelectPolygonFromCoordinates(Geometry::Point point) {
	// Loop over the the currently existing shapes
	for (int i = 0; i < this->shapes.size(); i++) {
		/*
		// Add the first point again to the end of the polygon as you can above the first and last point otherwise, does not seem to be treated as closed
		shapes[i]->renderCoordinates->outer().push_back(shapes[i]->renderCoordinates->outer()[0]);

		// Check if the given point is inside the polygon, if it is, set the current shape to selectedShape
		if (boost::geometry::within(point, shapes[i]->renderCoordinates->outer())){
			this->selectedShape = shapes[i];
			sidePanel->Update();

			shapes[i]->renderCoordinates->outer().pop_back();
			return true;
		}
		shapes[i]->renderCoordinates->outer().pop_back();
		*/

		if (shapes[i]->Select(point)) {
			this->selectedShape = shapes[i];
			return true;
		}
	}
	// If no shape is clicked then make sure no shape is selected
	this->selectedShape = NULL;
	return false;
}

std::vector <boost::shared_ptr<iconic::Geometry::Shape>> MeasureHandler::GetShapes() {
	return this->shapes;
}

boost::shared_ptr<iconic::Geometry::Shape> MeasureHandler::GetSelectedShape() {
	return this->selectedShape;
}


ReadOnlyMeasureHandler::ReadOnlyMeasureHandler(MeasureHandlerPtr ptr):mHandler(ptr){}
std::vector <boost::shared_ptr<iconic::Geometry::Shape>> ReadOnlyMeasureHandler::GetShapes() {
	return mHandler.get()->GetShapes();
}

boost::shared_ptr<iconic::Geometry::Shape> ReadOnlyMeasureHandler::GetSelectedShape() {
	return mHandler.get()->GetSelectedShape();
}