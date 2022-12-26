#include	<IconicMeasureCommon/Geometry.h>
#include	<wx/log.h>
#include	<wx/intl.h>

using namespace iconic;

Geometry::PolygonPtr Geometry::CreatePolygon(size_t n) {
	if (n == 0) {
		return PolygonPtr(new Polygon);
	}
	else {
		PolygonPtr p(new Polygon);
		p->outer().resize(n);
		return p;
	}
}

Geometry::Polygon3DPtr Geometry::CreatePolygon3D(size_t n) {
	if (n == 0) {
		return Polygon3DPtr(new Polygon3D);
	}
	else {
		Polygon3DPtr p(new Polygon3D);
		p->outer().resize(n);
		return p;
	}
}

bool Geometry::Initialize3DPolygon(const Geometry::PolygonPtr pImage, const Geometry::Polygon3DPtr pObject) {
	if (!pImage || !pObject) {
		wxLogError("No polygon");
		return false;
	}

	const size_t n = pImage->outer().size();
	if (!n) {
		wxLogError("No points in polygon");
		return false;
	}

	if (pObject->outer().size() != n) {
		// Make outer boundary of 3D polygon have same number of points as 2D polygon
		pObject->outer().resize(n);
	}

	const size_t nInterior = pImage->inners().size();
	if (!nInterior) {
		// Make sure 3D polygon has no interior either
		pObject->inners().clear();
		return true;
	}

	// Handle interior polygons 
	if (pObject->inners().size() != nInterior) {
		// Make same number of interior polygons in 3D as in 2D
		pObject->inners().resize(nInterior);
	}
	for (int i = 0; i < nInterior; ++i) {
		const std::vector<Point>& vIn = pImage->inners()[i];
		std::vector<Point3D>& vOut = pObject->inners()[i];
		const size_t nPoints = vIn.size();
		if (vOut.size() != nPoints) {
			// Make same number of points in interior 3D polygon as the 2D interior polygon
			vOut.resize(nPoints);
		}
	}

	return true;
}
