#include <IconicMeasureCommon/Geometry.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <Eigen/Core>

using namespace iconic;


Geometry::Geometry() : cCameraType(Camera::ECameraType::FULL)
{
	cImageSize[0] = cImageSize[1] = 0;
	cCameraToPixelTransform.setIdentity();
}

Geometry::PolygonPtr Geometry::CreatePolygon(size_t n)
{
	if (n == 0)
	{
		return PolygonPtr(new Polygon);
	}
	else
	{
		PolygonPtr p(new Polygon);
		p->outer().resize(n);
		return p;
	}
}

Geometry::Polygon3DPtr Geometry::CreatePolygon3D(size_t n)
{
	if (n == 0)
	{
		return Polygon3DPtr(new Polygon3D);
	}
	else
	{
		Polygon3DPtr p(new Polygon3D);
		p->outer().resize(n);
		return p;
	}
}

bool Geometry::Initialize3DPolygon(const Geometry::PolygonPtr pImage, const Geometry::Polygon3DPtr pObject)
{
	if (!pImage || !pObject)
	{
		wxLogError("No polygon");
		return false;
	}

	const size_t n = pImage->outer().size();
	if (!n)
	{
		wxLogError("No points in polygon");
		return false;
	}

	if (pObject->outer().size() != n)
	{
		// Make outer boundary of 3D polygon have same number of points as 2D polygon
		pObject->outer().resize(n);
	}

	const size_t nInterior = pImage->inners().size();
	if (!nInterior)
	{
		// Make sure 3D polygon has no interior either
		pObject->inners().clear();
		return true;
	}

	// Handle interior polygons
	if (pObject->inners().size() != nInterior)
	{
		// Make same number of interior polygons in 3D as in 2D
		pObject->inners().resize(nInterior);
	}
	for (int i = 0; i < nInterior; ++i)
	{
		const std::vector<Point>& vIn = pImage->inners()[i];
		std::vector<Point3D>& vOut = pObject->inners()[i];
		const size_t nPoints = vIn.size();
		if (vOut.size() != nPoints)
		{
			// Make same number of points in interior 3D polygon as the 2D interior polygon
			vOut.resize(nPoints);
		}
	}

	return true;
}

bool Geometry::ImageToObject(Geometry::PolygonPtr pImage, Geometry::Polygon3DPtr pObject)
{
	// Make 3D polygon same size as 2D polygon
	if (!Geometry::Initialize3DPolygon(pImage, pObject))
	{
		wxLogError("No polygon");
		return false;
	}

	// Transform outer boundary from image to object space
	if (!ImageToObject(pImage->outer(), pObject->outer()))
	{
		wxLogError("Could not transform outer image vector to object");
		return false;
	}

	// Transform inner boundaries from image to object space
	for (int i = 0; i < pImage->inners().size(); ++i)
	{
		const std::vector<Geometry::Point>& vIn = pImage->inners()[i];
		std::vector<Geometry::Point3D>& vOut = pObject->inners()[i];
		if (!ImageToObject(vIn, vOut))
		{
			wxLogError("Could not transform image point to object");
			return false;
		}
	}

	return true;
}

bool Geometry::ImageToObject(const std::vector<Geometry::Point>& vIn, std::vector<Geometry::Point3D>& vOut)
{
	for (int i = 0; i < vIn.size(); ++i)
	{
		if (!ImageToObject(vIn[i], vOut[i]))
		{
			wxLogError("Could not transform image point to object");
			return false;
		}
	}
	return true;
}

void Geometry::ImageToPixel(const Eigen::Vector2d& imagePt, Geometry::Point& pixelPt) const
{
	Eigen::Vector3d pixelPoint(imagePt[0], imagePt[1], 1.0); // image point in homogeneous coordinates
	pixelPoint = cCameraToPixelTransform * pixelPoint;		 // Transform from camera to pixel system
	pixelPt.set<0>(pixelPoint[0] / pixelPoint[2]);			 // Divide by last element to get euclidian coordinates
	pixelPt.set<1>(pixelPoint[1] / pixelPoint[2]);
}

void Geometry::ImageToPixel(const Geometry::Point& imagePt, Geometry::Point& pixelPt) const
{
	const Eigen::Vector2d cameraPt(imagePt.get<0>(), imagePt.get<1>()); // camera point
	ImageToPixel(cameraPt, pixelPt);
}

bool Geometry::GetZ(const int x, const int y, double& Z) const
{
	const size_t index = y * cImageSize[0] + x; // = index in 1D depth map vector for 2D coordinate (x,y)
	if (cDepthMap.size() <= index)
	{
		wxLogError(_("Image point (%d,%d) is out of range of depth map"), x, y);
		return false;
	}
	Z = cDepthMap[index];
	return true;
}

bool Geometry::ImageToObject(const Geometry::Point& pImage, Geometry::Point3D& pObject) const
{
	const Eigen::Vector2d cameraPt(pImage.get<0>(), pImage.get<1>()); // camera point as Eigen, used both to get Z and for Image2Object
	Geometry::Point pixelPt;
	ImageToPixel(cameraPt, pixelPt);
	int pixelCoord[] = { static_cast<int>(pixelPt.get<0>() + 0.5), static_cast<int>(pixelPt.get<1>() + 0.5) }; // Round to integers
	double Z = 0.0;
	if (!GetZ(pixelCoord[0], pixelCoord[1], Z))
	{
		wxLogError(_("Could not get heigth from depth map at (%d,%d)"), pixelCoord[0], pixelCoord[1]);
		return false;
	}
	if (Z > 1000.0)
	{
		wxLogError(_("Invalid height at (%d,%d)"), pixelCoord[0], pixelCoord[1]);
		return false;
	}
	Eigen::Vector4d X;
	if (!cpCamera->Image2Object(cameraPt, Z, X, cCameraType))
	{
		wxLogError(_("Could not compute image-to-object coordinates"));
		return false;
	}
	pObject.set<0>(X[0]);
	pObject.set<1>(X[1]);
	pObject.set<2>(X[2]);
	return true;
}

wxColour Geometry::GetColour(Colours c) const{
	//RED, GREEN, CYAN, MAGENTA, YELLOW, CERISE
	wxColour const cols[] = { wxColor(255, 10, 10, 150), wxColor(10, 255, 10, 150), wxColor(10, 255, 255, 150), wxColor(255, 10, 255, 150), wxColor(255, 255, 10, 150), wxColor(238, 42, 123, 155)};

	return cols[(int)c];
}