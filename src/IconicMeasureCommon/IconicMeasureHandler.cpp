#include	<IconicMeasureCommon/IconicMeasureHandler.h>
#include	<wx/filename.h>
#include	<wx/log.h>
#include	<wx/ffile.h>

using namespace iconic;

IconicMeasureHandler::IconicMeasureHandler() :
	cbIsParsed(false)
{
}

bool IconicMeasureHandler::OnNextFrame(iconic::gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse) {
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

wxString IconicMeasureHandler::GetMetaFileName() const {
	return cDepthMapFileName;
}

bool IconicMeasureHandler::Parse() {
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

bool IconicMeasureHandler::ReadDepthMap() {
	if (!wxFileName::FileExists(cDepthMapFileName)) {
		wxLogError("Depth map is missing (%s)", cDepthMapFileName);
		return false;
	}

	size_t imSize[2];
	GetImageSize(imSize[0], imSize[1]);
	if (!imSize[0] || !imSize[1]) {
		wxLogError(_("Unknown image size"));
		return false;
	}

	const size_t nPixels = imSize[0] * imSize[1];
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

bool IconicMeasureHandler::ReadCamera() {
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

	CheckCamera();

	cbIsParsed = true;
	return true;
}

void IconicMeasureHandler::CheckCamera() {
	if (!cpCamera) {
		cCameraType = ECameraType::FULL;
		return;
	}
	cpCamera->Decompose(false);
	if (cpCamera->cX0 != Eigen::Vector3d(0.0, 0.0, 0.0)) {
		// Translation is not zero
		cCameraType = ECameraType::FULL;
		return;
	}
	Eigen::Vector3d angles;
	cpCamera->cR.GetAngles(angles, true);
	if (angles != Eigen::Vector3d(0.0, 0.0, 0.0)) {
		// Rotation is not zero
		cCameraType = ECameraType::FULL;
		return;
	}
	Eigen::Matrix3d& K = cpCamera->cK;
	if (K(0, 1) != 0.0 || K(0, 2) != 0.0 || K(1, 2) != 0.0 || K(0,0) != K(1,1)) {
		// Calibration matrix has more parameters than focal length or focal length in x and y differ
		cCameraType = ECameraType::CALIBRATION_MATRIX;
		return;
	}
	if (K(0, 0) == 1.0) {
		// Identity camera
		cCameraType = ECameraType::IDENTITY;
	}
	else {
		// Only focal length is defined
		cCameraType = ECameraType::FOCAL_LENGTH;
	}
}

void IconicMeasureHandler::AddImagePolygon(PolygonPtr pPolygon, bool bAddObjectPolygon) {
	if (bAddObjectPolygon) {
		Polygon3DPtr pObject(new Polygon3D);
		if (!ImageToObject(pPolygon, pObject)) {
			wxLogError("Could not transform image polygon to object space");
			return;
		}
		cvObjectPolygon.push_back(pObject);
	}
	cvImagePolygon.push_back(pPolygon);
}

bool IconicMeasureHandler::ImageToObject(PolygonPtr pImage, Polygon3DPtr pObject) {
	if (!pImage || !pObject) {
		wxLogError("No polygon");
		return false;
	}
	size_t n = pImage->outer().size();
	if (!n) {
		wxLogError("No points in polygon");
		return false;
	}

	if (pObject->outer().size() != n) {
		pObject->outer().resize(n);
	}
	for (int i = 0; i < n; ++i) {
		const Point& p = pImage->outer()[i];
		Point3D& P = pObject->outer()[i];
		if (!ImageToObject(p, P)) {
			wxLogError("Could not transform image point to object");
			return false;
		}
	}

	return true;
}

bool IconicMeasureHandler::ImageToObject(const Point& pImage, Point3D& pObject) {
	// ToDo Use camera and depth map to transform (col,row,Z) to (X,Y,Z) 
	// Meanwhile just copy the image coordinates:
	pObject.set<0>(pImage.get<0>());
	pObject.set<1>(pImage.get<1>());
	return true;
}

IconicMeasureHandler::PolygonPtr IconicMeasureHandler::CreatePolygon(size_t n) {
	if (n == 0) {
		return PolygonPtr(new Polygon);
	}
	else {
		PolygonPtr p(new Polygon);
		p->outer().resize(n);
		return p;
	}
}

IconicMeasureHandler::Polygon3DPtr IconicMeasureHandler::CreatePolygon3D(size_t n) {
	if (n == 0) {
		return Polygon3DPtr(new Polygon3D);
	}
	else {
		Polygon3DPtr p(new Polygon3D);
		p->outer().resize(n);
		return p;
	}
}

CameraPtr IconicMeasureHandler::GetCamera() {
	if (!cpCamera) {
		cpCamera = boost::make_shared<Camera>();
	}
	return cpCamera;
}

std::vector<float>& IconicMeasureHandler::GetDepthMap() {
	return cDepthMap;
}

void IconicMeasureHandler::GetImageSize(size_t &width, size_t &height) {
	if (!cpProperties) {
		width = height = 0;
	}
	cpProperties->GetImageSize(width, height);
}
