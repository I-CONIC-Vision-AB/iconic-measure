#include	<IconicMeasureCommon/IconicMeasureHandler.h>
#include	<wx/filename.h>
#include	<wx/log.h>

IconicMeasureHandler::IconicMeasureHandler() :
	cbIsParsed(false)
{

}

bool IconicMeasureHandler::OnNextFrame(iconic::gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse) {
	cpProperties = pProperties;
	cImageFileName = filename;
	wxFileName fn(cImageFileName);
	fn.SetExt("dmap");
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
	if (!wxFileName::FileExists(cDepthMapFileName)) {
		wxLogError("Depth map is missing (%s)", cDepthMapFileName);
		return false;
	}
	if (!wxFileName::FileExists(cCameraFileName)) {
		wxLogError("Camera file is missing (%s)", cCameraFileName);
		return false;
	}
	return true;
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
