#include	<IconicMeasureCommon/MeasureHandler.h>
#include	<IconicSensor/Camera.h>
#include	<wx/filename.h>
#include	<wx/log.h>
#include	<wx/ffile.h>

using namespace iconic;

MeasureHandler::MeasureHandler() :
	cbIsParsed(false),
	cCameraType(Camera::ECameraType::FULL)
{
	cImageSize[0] = cImageSize[1] = 0;
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
	cpProperties->GetImageSize(cImageSize[0], cImageSize[1]);

	const size_t nPixels = cImageSize[0] * cImageSize[1];
	if (cDepthMap.size() != nPixels) {
		cDepthMap.resize(nPixels);
	}
	wxFFile file(cDepthMapFileName, "rb");
	if (!file.IsOpened()) {
		wxLogError(_("Could not open %s"), cDepthMapFileName);
		return false;
	}
	if (file.Read(cDepthMap.data(), sizeof(float) * nPixels) != sizeof(float) * nPixels) {
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

	Camera::Camera2PixelMatrix(cImageSize[0], cImageSize[1], cCameraToPixelTransform);

	CheckCamera();

	cbIsParsed = true;
	return true;
}

void MeasureHandler::CheckCamera() {
	if (!cpCamera) {
		cCameraType = Camera::ECameraType::FULL;
		return;
	}
	cCameraType = cpCamera->ClassifyCamera();
}

void MeasureHandler::AddImagePolygon(Geometry::PolygonPtr pPolygon, bool bAddObjectPolygon) {
	if (bAddObjectPolygon) {
		Geometry::Polygon3DPtr pObject(new Geometry::Polygon3D);
		if (!ImageToObject(pPolygon, pObject)) {
			wxLogError("Could not transform image polygon to object space");
			return;
		}
		cvObjectPolygon.push_back(pObject);
	}
	cvImagePolygon.push_back(pPolygon);
}

bool MeasureHandler::ImageToObject(Geometry::PolygonPtr pImage, Geometry::Polygon3DPtr pObject) {
	// Make 3D polygon same size as 2D polygon
	if (!Geometry::Initialize3DPolygon(pImage, pObject)) {
		wxLogError("No polygon");
		return false;
	}

	// Transform outer boundary from image to object space
	if (!ImageToObject(pImage->outer(), pObject->outer())) {
		wxLogError("Could not transform outer image vector to object");
		return false;
	}

	// Transform inner boundaries from image to object space
	for (int i = 0; i < pImage->inners().size(); ++i) {
		const std::vector<Geometry::Point>& vIn = pImage->inners()[i];
		std::vector<Geometry::Point3D>& vOut = pObject->inners()[i];
		if (!ImageToObject(vIn, vOut)) {
			wxLogError("Could not transform image point to object");
			return false;
		}
	}

	return true;
}

bool MeasureHandler::ImageToObject(const std::vector<Geometry::Point>& vIn, std::vector<Geometry::Point3D>& vOut) {
	for (int i = 0; i < vIn.size(); ++i) {
		if (!ImageToObject(vIn[i], vOut[i])) {
			wxLogError("Could not transform image point to object");
			return false;
		}
	}
	return true;
}

bool MeasureHandler::ImageToObject(const Geometry::Point& pImage, Geometry::Point3D& pObject) {
	const Eigen::Vector2d cameraPt(pImage.get<0>(), pImage.get<1>());	// camera point
	Eigen::Vector3d pixelPoint(cameraPt[0], cameraPt[1], 1.0);			// image point
	pixelPoint = cCameraToPixelTransform * pixelPoint;					// Transform from camera to pixel system
	pixelPoint /= pixelPoint[2];										// Divide by last element to get euclidian coordinates
	int pixelCoord[] = { static_cast<int>(pixelPoint[0] + 0.5), static_cast<int>(pixelPoint[1] + 0.5) }; // Round to integers
	const size_t index = pixelCoord[1] * cImageSize[0] + pixelCoord[0]; // = y * width + x = index in 1D pixel for 2D coordinate (x,y)
	if (cDepthMap.size() <= index) {
		wxLogError(_("Image point (%d,%d) is out of range of depth map"), pImage.get<0>(), pImage.get<1>());
		return false;
	}
	const double Z = cDepthMap[index];
	if (Z > 1000.0) {
		wxLogError(_("Invalid height at (%d,%d)"), pImage.get<0>(), pImage.get<1>());
		return false;
	}
	Eigen::Vector4d X;
	if (!cpCamera->Image2Object(cameraPt, Z, X, cCameraType)) {
		wxLogError(_("Could not compute image-to-object coordinates"));
		return false;
	}
	pObject.set<0>(X[0]);
	pObject.set<1>(X[1]);
	pObject.set<2>(X[2]);
	return true;
}

CameraPtr MeasureHandler::GetCamera() {
	if (!cpCamera) {
		cpCamera = boost::make_shared<Camera>();
	}
	return cpCamera;
}

std::vector<float>& MeasureHandler::GetDepthMap() {
	return cDepthMap;
}

void MeasureHandler::GetImageSize(size_t& width, size_t& height) {
	width = cImageSize[0];
	height = cImageSize[1];
}
