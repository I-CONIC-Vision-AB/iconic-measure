#include <IconicMeasureCommon/MeasureHandler.h>
#include <IconicSensor/Camera.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/ffile.h>

using namespace iconic;

MeasureHandler::MeasureHandler() : cbIsParsed(false)
{
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

void MeasureHandler::AddImagePolygon(Geometry::PolygonPtr pPolygon, bool bAddObjectPolygon)
{
	if (bAddObjectPolygon)
	{
		Geometry::Polygon3DPtr pObject(new Geometry::Polygon3D);
		if (!cGeometry.ImageToObject(pPolygon, pObject))
		{
			wxLogError("Could not transform image polygon to object space");
			return;
		}
		cvObjectPolygon.push_back(pObject);
		shapes.push_back(boost::shared_ptr<iconic::Geometry::Shape>(new iconic::Geometry::Shape(iconic::Geometry::ShapeType::PolygonShape, pObject)));
	}
	cvImagePolygon.push_back(pPolygon);
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

bool MeasureHandler::AddPointToSelectedShape(iconic::Geometry::Point3D p, boost::compute::float2_ imgP) {
	// If this is a brand new shape, instantiate it
	if (!this->selectedShape) {
		this->selectedShape = boost::shared_ptr<iconic::Geometry::Shape>(new iconic::Geometry::Shape(iconic::Geometry::ShapeType::PolygonShape, cGeometry.CreatePolygon3D(1)));
		this->shapes.push_back(this->selectedShape);
	}
	this->selectedShape->dataPointer.get()->outer().push_back(p);
	this->selectedShape->renderCoordinates.push_back(imgP);
	
	wxLogVerbose(_("There are currently " + std::to_string(this->shapes.size()) + " number of shapes"));

	return true; // Temporary solution
}

void MeasureHandler::HandleFinishedMeasurement() {
	if (true){//std::find(this->shapes.begin(), this->shapes.end(), *(this->selectedShape)) != this->shapes.end()) {
		// It is a new shape
		this->selectedShape = NULL;
	}
	else {
		// Do nothing for the moment
		// TODO: Make sure that everything works when created shapes are selectable
	}
}

// Bug: Can currently select polygons when clicking above them
bool MeasureHandler::SelectPolygonFromCoordinates(boost::compute::float2_ p) {
	iconic::Geometry::Point point = iconic::Geometry::Point(p.x, p.y);

	// If there is a previously selectedShape set it to the unselected color
	if (this->selectedShape) {
		this->selectedShape->color = iconic::Geometry::Color{ 255, 0, 0, 255 };
	}

	// Loop over the the currently existing shapes
	for (int i = 0; i < this->shapes.size(); i++) {
		// Create a temporary polygon to compare with. This is done because you cannot use a Polygon3D in boost::geometry::within()

		iconic::Geometry::Polygon polygon;
		for (int j = 0; j < shapes[i]->renderCoordinates.size(); j++) {
			iconic::Geometry::Point temp_p = iconic::Geometry::Point(shapes[i]->renderCoordinates[j].x, shapes[i]->renderCoordinates[j].y);
			polygon.outer().push_back(temp_p);
		}
		// Add the first point again to the end of the polygon as you can above the first and last point otherwise, does not seem to be treated as closed
		polygon.outer().push_back(iconic::Geometry::Point(shapes[i]->renderCoordinates[0].x, shapes[i]->renderCoordinates[0].y));

		wxLogStatus("Tried to select");

		// Check if the given point is inside the polygon, if it is, set the current shape to selectedShape
		if (boost::geometry::within(point, polygon)) {
			wxLogStatus("Selected something");
			this->selectedShape = shapes[i];
			this->selectedShape->color = iconic::Geometry::Color { 0, 255, 0, 255 };
			return true;
		}
	}
	return false;
}

std::vector <boost::shared_ptr<iconic::Geometry::Shape>> MeasureHandler::GetShapes() {
	return this->shapes;
}


ReadOnlyMeasureHandler::ReadOnlyMeasureHandler(MeasureHandlerPtr ptr):mHandler(ptr){}
std::vector <boost::shared_ptr<iconic::Geometry::Shape>> ReadOnlyMeasureHandler::GetShapes() {
	return mHandler.get()->GetShapes();
}