#include <IconicMeasureCommon/Shape.h>
#include <IconicMeasureCommon/Geometry.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <tesselator.h>
#include <GL/glew.h>
#include <IconicGpu/Triangulator.h>

using namespace iconic;

// Constructors ---------------------------------------------------------------------------
Shape::Shape(ShapeType t, wxColour c) {
	cType = t;
	cColor = c;
	cSelectedPointIndex = -1;
	cNextInsertIndex = 0;
	cFinished = false;
};

Shape::~Shape() {}

PointShape::PointShape(wxColour c) : Shape(ShapeType::PointType, c) {
	cRenderCoordinate = Geometry::Point(-1, -1);
	cCoordinate = Geometry::Point3D(-1, -1, -1);
	cIsComplete = false;
}
PointShape::PointShape(wxColour c, wxString& wkt) : Shape(ShapeType::PointType, c) {
	boost::geometry::read_wkt(wkt.ToStdString(), cRenderCoordinate);
	cIsComplete = true;
}
PointShape::~PointShape() {}

LineShape::LineShape(wxColour c) : 
	Shape(ShapeType::LineType, c),
	cRenderCoordinates(new Geometry::VectorTrain),
	cCoordinates(new Geometry::VectorTrain3D)
{}
LineShape::LineShape(wxColour c, wxString& wkt) : 
	Shape(ShapeType::LineType, c),
	cRenderCoordinates(new Geometry::VectorTrain),
	cCoordinates(new Geometry::VectorTrain3D)
{
	cRenderCoordinates = Geometry::VectorTrainPtr(new Geometry::VectorTrain);
	cCoordinates = Geometry::VectorTrain3DPtr(new Geometry::VectorTrain3D);
	boost::geometry::read_wkt(wkt.ToStdString(), *cRenderCoordinates.get());
}
LineShape::~LineShape() {}

PolygonShape::PolygonShape(wxColour c) :
	Shape(ShapeType::PolygonType, c),
	cpTesselator(nullptr),
	cRenderCoordinates(new Geometry::Polygon),
	cCoordinates(new Geometry::Polygon3D) {
	SetDrawMode();
}
PolygonShape::PolygonShape(wxColour c, wxString& wkt) :
	Shape(ShapeType::PolygonType, c),
	cpTesselator(nullptr),
	cRenderCoordinates(new Geometry::Polygon),
	cCoordinates(new Geometry::Polygon3D)
{
	boost::geometry::read_wkt(wkt.ToStdString(), *cRenderCoordinates.get());
	SetDrawMode();
	if (cRenderCoordinates) {
		Tesselate();
	}
}

PolygonShape::PolygonShape(Geometry::PolygonPtr pPolygon, wxColour c) : Shape(ShapeType::PolygonType, c),
cpTesselator(nullptr) {
	cRenderCoordinates = pPolygon;
	cCoordinates = Geometry::Polygon3DPtr(new Geometry::Polygon3D);
	SetDrawMode();
	if (cRenderCoordinates) {
		Tesselate();
	}
}

PolygonShape::~PolygonShape() {
	if (cpTesselator) {
		tessDeleteTess(cpTesselator);
		cpTesselator = nullptr;
	}
}

// GetCoordinate ------------------------------------------------------------------------
void PointShape::GetCoordinate(Geometry::Point3D& coordinate) {
	coordinate.set<0>(cCoordinate.get<0>());
	coordinate.set<1>(cCoordinate.get<1>());
	coordinate.set<2>(cCoordinate.get<2>());
}
void LineShape::GetCoordinate(Geometry::Point3D& coordinate) {}
void PolygonShape::GetCoordinate(Geometry::Point3D& coordinate) {}

// GetArea ------------------------------------------------------------------------------
double PointShape::GetArea() {
	return -1;
}
double LineShape::GetArea() {
	return -1;
}
double PolygonShape::GetArea() {
	return cArea;
}
// GetLength -------------------------------------------------------------
double PointShape::GetLength() {
	return -1;
}
double LineShape::GetLength() {
	return cLength;
}
double PolygonShape::GetLength() {
	return cLength;
}
// GetVolume -------------------------------------------------------------
double PointShape::GetVolume() {
	return -1;
}
double LineShape::GetVolume() {
	return -1;
}
double PolygonShape::GetVolume() {
	return cVolume;
}
//GetHeightProfile ---------------------------------------------------
boost::shared_ptr<Geometry::HeightProfile> PointShape::GetHeightProfile() {
	return NULL;
}
boost::shared_ptr<Geometry::HeightProfile> LineShape::GetHeightProfile() {
	return cProfile;
}
boost::shared_ptr<Geometry::HeightProfile> PolygonShape::GetHeightProfile() {
	return NULL;
}
// Select -------------------------------------------------------------
bool PointShape::Select(Geometry::Point mouseClick) {
	// Add ImageCanvas::GetScale() as argument?
	if (boost::geometry::distance(mouseClick, cRenderCoordinate) < 0.005f) { // Should depend on the zoom amount
		return true;
	} else {
		return false;
	}
}
bool LineShape::Select(Geometry::Point mouseClick) {
	if (boost::geometry::distance(mouseClick, *cRenderCoordinates) < 0.001f) { // Should depend on the zoom amount
		return true;
	} else {
		return false;
	}
}
bool PolygonShape::Select(Geometry::Point mouseClick) {
	return boost::geometry::within(mouseClick, *cRenderCoordinates);
}
// GetPoint -------------------------------------------------------------
bool PointShape::GetPoint(Geometry::Point mouseClick) {
	return boost::geometry::distance(mouseClick, cRenderCoordinate) < 0.005f;
}
bool LineShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : *cRenderCoordinates) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			cSelectedPointIndex = i;
			return true;
		}
		i++;
	}
	return false;
}
bool PolygonShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : cRenderCoordinates->outer()) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			cSelectedPointIndex = i;
			wxLogVerbose(_("Selected index: " + std::to_string(cSelectedPointIndex)));
			return true;
		}
		i++;
	}
	return false;
}
// GetRenderingPoint ---------------------------------------------------
Geometry::Point PointShape::GetRenderingPoint(int index) {
	return cRenderCoordinate;
}
Geometry::Point LineShape::GetRenderingPoint(int index) {
	if (index == -1 || index >= GetNumberOfPoints())
		return cRenderCoordinates->back();
	else
		return cRenderCoordinates->at(index);
}
Geometry::Point PolygonShape::GetRenderingPoint(int index) {
	if (index >= GetNumberOfPoints())
		return cRenderCoordinates->outer().at(0);
	if (index != 0 && !IsCompleted())
		return cRenderCoordinates->outer().back();
	else if (index == -1)
		return cRenderCoordinates->outer().at(GetNumberOfPoints() - 2); // Skip the last point that is identical to the first
	else
		return cRenderCoordinates->outer().at(index);
}

// Draw ---------------------------------------------------------------
void PointShape::Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) {
	glPushAttrib(GL_CURRENT_BIT);	// Apply color until pop
	glColor3ub(GetColor().Red(), GetColor().Green(), GetColor().Blue());		  // Color of geometry
	(selected) ? glPointSize(20.f) : glPointSize(10.f);
	glBegin(GL_POINTS);
	glVertex2f(cRenderCoordinate.get<0>(), cRenderCoordinate.get<1>());
	glEnd();
	glPopAttrib();
}
void LineShape::Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) {
	wxColour color = GetColor();
	// Draw the measured points
	glPushAttrib(GL_CURRENT_BIT); // Apply color until pop
	glColor3ub(color.Red(), color.Green(), color.Blue());		  // Color of geometry
	glLineWidth(3.f);
	glBegin(GL_LINE_STRIP);

	Geometry::Point p;
	for (size_t i = 0; i < GetNumberOfPoints(); i++) {
		p = GetRenderingPoint(i);
		glVertex2f(p.get<0>(), p.get<1>());
	}
	glEnd();

	if (selected) {
		glPointSize(10.f);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < GetNumberOfPoints(); i++) {
			p = GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
	}
	if (isMeasuring) {

		// Draw the mouse track
		glBegin(GL_LINE_LOOP);
		int i = GetPossibleIndex(mousePoint);
		if (i == 0) {
			glVertex2f(GetRenderingPoint(0).get<0>(), GetRenderingPoint(0).get<1>());
			glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());
		} else {
			glVertex2f(GetRenderingPoint(cNextInsertIndex - 1).get<0>(), GetRenderingPoint(cNextInsertIndex - 1).get<1>());
			glVertex2f(GetRenderingPoint(cNextInsertIndex).get<0>(), GetRenderingPoint(cNextInsertIndex).get<1>());
			glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());
		}

		glEnd();
	}

	glPopAttrib();
}
void PolygonShape::Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) {
	glPushAttrib(GL_CURRENT_BIT);	// Apply color until pop
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw mouse track
	if (isMeasuring) {
		glColor3ub(cColor.Red(), cColor.Green(), cColor.Blue());
		GetPossibleIndex(mousePoint);
		glColor3ub(cColor.Red(), cColor.Green(), cColor.Blue());			// Color of geometry
		glLineWidth(3.f);
		glBegin(GL_LINE_LOOP);


		glVertex2f(GetRenderingPoint(cNextInsertIndex - 1).get<0>(), GetRenderingPoint(cNextInsertIndex - 1).get<1>());
		glVertex2f(GetRenderingPoint(cNextInsertIndex).get<0>(), GetRenderingPoint(cNextInsertIndex).get<1>());
		glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());

		glEnd();
	}

	if (!IsCompleted()) {
		glColor3ub(cColor.Red(), cColor.Green(), cColor.Blue());
		glLineWidth(3.f);
		glPointSize(10.f);
		Geometry::Point p;

		glBegin(GL_LINE_STRIP);

		for (size_t i = 0; i < GetNumberOfPoints(); i++) {
			p = GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
		glBegin(GL_POINTS);
		for (size_t i = 0; i < GetNumberOfPoints(); i++) {
			p = GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
		glPopAttrib();
		return;
	}
	if (!cpTesselator) Tesselate();

	// Get tesselated pieces.
	const float* verts = tessGetVertices(cpTesselator);
	const int* elems = tessGetElements(cpTesselator);
	const int nverts = tessGetVertexCount(cpTesselator);
	const int nelems = tessGetElementCount(cpTesselator);

	int i, j;
	const int triangles = 3;
	if (cbDrawPolygon && !selected) {
		// Draw polygons.
		glColor4ub(cColor.Red(), cColor.Green(), cColor.Blue(), selected ? cColor.Alpha() : cColor.Alpha() / 2);
		for (i = 0; i < nelems; ++i) {
			const int* p = &elems[i * triangles];
			glBegin(GL_TRIANGLE_FAN);
			for (j = 0; j < triangles && p[j] != TESS_UNDEF; ++j)
				glVertex2f(verts[p[j] * 2], verts[p[j] * 2 + 1]);
			glEnd();
		}
	}

	if (cbDrawLines && selected) {
		glColor4ub(0, 0, 128, 64);
		glLineWidth(3.0f);
		for (i = 0; i < nelems; ++i) {
			const int* p = &elems[i * triangles];
			glBegin(GL_LINE_LOOP);
			for (j = 0; j < triangles && p[j] != TESS_UNDEF; ++j)
				glVertex2f(verts[p[j] * 2], verts[p[j] * 2 + 1]);
			glEnd();
		}
	}

	if (!selected) {
		glColor3ub(cColor.Red(), cColor.Green(), cColor.Blue());
		glLineWidth(3.f);
		glBegin(GL_LINE_LOOP);

		Geometry::Point p;
		for (size_t i = 0; i < GetNumberOfPoints(); i++) {
			p = GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
	}

	if (cbDrawPoints || selected) {
		glColor3ub(cColor.Red(), cColor.Green(), cColor.Blue());
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		for (i = 0; i < nverts; ++i) {
			glVertex2f(verts[i * 2], verts[i * 2 + 1]);
		}
		glEnd();
	}

	glPopAttrib();
}
// AddPoint ------------------------------------------------------------
bool PointShape::AddPoint(Geometry::Point newPoint, int index) {
	if (cIsComplete) return GetPoint(newPoint); // If the point is completed the point should be selectable, but no new points can be added
	else {
		cRenderCoordinate = newPoint;
		cIsComplete = true;
		// UpdateCalculations should be called after the point has been defined
		return true;
	}
}
bool LineShape::AddPoint(Geometry::Point newPoint, int index) {
	if (GetPoint(newPoint)) return true;

	if (IsCompleted() && cNextInsertIndex < GetNumberOfPoints()) {
		cRenderCoordinates->insert(cRenderCoordinates->begin() + cNextInsertIndex, newPoint);
		cSelectedPointIndex = cNextInsertIndex;
	} else {
		cRenderCoordinates->push_back(newPoint);
		cSelectedPointIndex = GetNumberOfPoints() - 1;
	}
	return true;
}
bool PolygonShape::AddPoint(Geometry::Point newPoint, int index) {
	if (IsCompleted() && GetPoint(newPoint)) { // See if a point could be selected before creating a new one
		return true;
	}

	if (IsCompleted()) {
		if (cNextInsertIndex == 0) {
			cRenderCoordinates->outer().pop_back();
		}

		cRenderCoordinates->outer().insert(cRenderCoordinates->outer().begin() + cNextInsertIndex, newPoint);
		cSelectedPointIndex = cNextInsertIndex;
		if (cNextInsertIndex == 0) {
			boost::geometry::correct(*(cRenderCoordinates));
		}
		Tesselate();
	} else {
		// Default if the polygon is not yet a polygon (i.e. has less than 3 points)
		cRenderCoordinates->outer().push_back(newPoint);
		cSelectedPointIndex = GetNumberOfPoints() - 1;
		if (IsCompleted())
			cRenderCoordinates->outer().push_back(cRenderCoordinates->outer().front());
	}

	return true;
}
//UpdateCalculations -----------------------------------------------------------
void PointShape::UpdateCalculations(Geometry& g) {
	if (!cIsComplete) return;
	if (!g.ImageToObject(cRenderCoordinate, cCoordinate)) {
		wxLogError(_("Could not compute image-to-object coordinates for measured point"));
		return;
	}
}
void LineShape::UpdateCalculations(Geometry& g) {
	if (!IsCompleted()) return;
	cCoordinates->clear();
	Geometry::Point3D p;
	for (int i = 0; i < cRenderCoordinates->size(); i++) {
		if (!g.ImageToObject(cRenderCoordinates->at(i), p)) {
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
		cCoordinates->push_back(p);
	}
	double currLen;
	cLength = 0;
	for (int i = 1; i < cCoordinates->size(); i++) {
		currLen = pow(cCoordinates->at(i).get<0>() - cCoordinates->at(i - 1).get<0>(), 2) + pow(cCoordinates->at(i).get<1>() - cCoordinates->at(i - 1).get<1>(), 2) + pow(cCoordinates->at(i).get<2>() - cCoordinates->at(i - 1).get<2>(), 2);
		cLength += sqrt(currLen);
	}

	//Code for calculating the heightprofile should go here
}
void PolygonShape::UpdateCalculations(Geometry& g) {
	//boost::geometry::correct(*(cRenderCoordinates));

	cCoordinates->clear();
	Geometry::Point3D objectPt;
	for (int i = 0; i < GetNumberOfPoints(); i++) {
		if (!g.ImageToObject(cRenderCoordinates->outer().at(i), objectPt)) {
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
		cCoordinates->outer().push_back(objectPt);
	}
	cLength = boost::geometry::perimeter(cRenderCoordinates->outer());
	cArea = boost::geometry::area(cRenderCoordinates->outer());
	cVolume = cArea * 5; // Not a correct solution
}
void PolygonShape::Tesselate() {
	if (IsCompleted()) {
		if (cpTesselator) {
			tessDeleteTess(cpTesselator);
		}
		cpTesselator = tessNewTess(nullptr);

		// Tesselate exterior boundary
		tessSetOption(cpTesselator, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);
		size_t nPoints = cRenderCoordinates->outer().size();
		std::vector<float> vFloatPoints(2 * nPoints);
		for (int i = 0; i < nPoints; ++i) {
			const Geometry::Point& p = cRenderCoordinates->outer().at(i);
			vFloatPoints[2 * i] = p.get<0>();
			vFloatPoints[2 * i + 1] = p.get<1>();
		}
		tessAddContour(cpTesselator, 2, vFloatPoints.data(), sizeof(float) * 2, nPoints);

		// Tesselate holes in polygon if any
		for (int j = 0; j < cRenderCoordinates->inners().size(); ++j) {
			nPoints = cRenderCoordinates->inners()[j].size();
			if (vFloatPoints.size() < (2 * nPoints)) {
				vFloatPoints.resize(2 * nPoints);
			}
			for (int i = 0; i < nPoints; ++i) {
				const Geometry::Point& p = cRenderCoordinates->inners()[j].at(i);
				vFloatPoints[2 * i] = p.get<0>();
				vFloatPoints[2 * i + 1] = p.get<1>();
			}
			tessAddContour(cpTesselator, 2, vFloatPoints.data(), sizeof(float) * 2, nPoints);
		}

		const int nvp = 3;
		tessTesselate(cpTesselator, TESS_WINDING_POSITIVE, TESS_POLYGONS, nvp, 2, 0);
	}
}
// GetNumberOfPoints ---------------------------------------------------------------------
int PointShape::GetNumberOfPoints() {
	return 1;
}
int LineShape::GetNumberOfPoints() {
	return cRenderCoordinates->size();
}
int PolygonShape::GetNumberOfPoints() {
	return cRenderCoordinates->outer().size();
}

// IsCompleted -------------------------------------------------------------
bool PointShape::IsCompleted() {
	return cIsComplete;
}
bool LineShape::IsCompleted() {
	return cRenderCoordinates->size() > 1;
}
bool PolygonShape::IsCompleted() {
	return cRenderCoordinates->outer().size() > 2;
}
// GetPossibleIndex -----------------------------------------------------------------------
int PointShape::GetPossibleIndex(Geometry::Point mousePoint) {
	return 0;
}
int LineShape::GetPossibleIndex(Geometry::Point mousePoint) {
	if (!IsCompleted()) return 0;
	if (!cFinished) return cNextInsertIndex = GetNumberOfPoints() + 1;
	int shortestIndex = 0;
	double shortestDistance = boost::geometry::distance(cRenderCoordinates->at(0), mousePoint);
	double currentDistance = 0;
	for (size_t i = 0; i < cRenderCoordinates->size(); i++) {
		currentDistance = boost::geometry::distance(cRenderCoordinates->at(i), mousePoint);
		if (currentDistance < shortestDistance) {
			shortestDistance = currentDistance;
			shortestIndex = i;
		}
	}

	double preDistance, postDistance = 0;
	preDistance = boost::geometry::distance(GetRenderingPoint(shortestIndex - 1), mousePoint); // GetRenderPoint loops back to the last element for index -1
	if (shortestIndex == 0) {
		Geometry::Point diff1(GetRenderingPoint(1).get<0>() - GetRenderingPoint(0).get<0>(), GetRenderingPoint(1).get<1>() - GetRenderingPoint(0).get<1>());
		Geometry::Point diff2(mousePoint.get<0>() - GetRenderingPoint(0).get<0>(), mousePoint.get<1>() - GetRenderingPoint(0).get<1>());
		if (boost::geometry::dot_product(diff1, diff2) > 0) return cNextInsertIndex = shortestIndex + 1;
		else return cNextInsertIndex = shortestIndex;
	} else if (shortestIndex == GetNumberOfPoints() - 1) {
		Geometry::Point diff1(GetRenderingPoint(shortestIndex - 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex - 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(mousePoint.get<0>() - GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff2) > 0) return cNextInsertIndex = shortestIndex;
		else return cNextInsertIndex = shortestIndex + 1;
	} else {
		Geometry::Point diff1(GetRenderingPoint(shortestIndex - 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex - 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(GetRenderingPoint(shortestIndex + 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex + 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return cNextInsertIndex = shortestIndex;
		else return cNextInsertIndex = shortestIndex + 1;
	}
}
int PolygonShape::GetPossibleIndex(Geometry::Point mousePoint) {
	if (!IsCompleted()) return 0;
	if (!cFinished) return cNextInsertIndex = GetNumberOfPoints() - 1;
	int shortestIndex = 0;
	double shortestDistance = boost::geometry::distance(cRenderCoordinates->outer().at(0), mousePoint);
	double currentDistance = 0;
	for (size_t i = 0; i < GetNumberOfPoints(); i++) {
		currentDistance = boost::geometry::distance(cRenderCoordinates->outer().at(i), mousePoint);
		if (currentDistance < shortestDistance) {
			shortestDistance = currentDistance;
			shortestIndex = i;
		}
	}
	double preDistance, postDistance = 0;
	preDistance = boost::geometry::distance(GetRenderingPoint(shortestIndex - 1), mousePoint); // GetRenderPoint loops back to the last element for index -1
	if (shortestIndex == 0) {
		Geometry::Point diff1(GetRenderingPoint(GetNumberOfPoints() - 2).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(GetNumberOfPoints() - 2).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(GetRenderingPoint(shortestIndex + 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex + 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return cNextInsertIndex = shortestIndex;
		else return cNextInsertIndex = shortestIndex + 1;

	} else if (shortestIndex == GetNumberOfPoints() - 1) {
		Geometry::Point diff1(GetRenderingPoint(shortestIndex - 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex - 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(GetRenderingPoint(1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return cNextInsertIndex = shortestIndex;
		else return cNextInsertIndex = shortestIndex + 1;
	} else {
		Geometry::Point diff1(GetRenderingPoint(shortestIndex - 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex - 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(GetRenderingPoint(shortestIndex + 1).get<0>() - GetRenderingPoint(shortestIndex).get<0>(), GetRenderingPoint(shortestIndex + 1).get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return cNextInsertIndex = shortestIndex;
		else return cNextInsertIndex = shortestIndex + 1;
	}
}
// DeselectPoint --------------------------------------------------------------------
void PointShape::DeselectPoint() {
	cIsComplete = true;
}
void LineShape::DeselectPoint() {
	cSelectedPointIndex = -1;
}
void PolygonShape::DeselectPoint() {
	cSelectedPointIndex = -1;
}

// MoveSelectedPoint -----------------------------------------------------------------
void PointShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	cRenderCoordinate = mousePoint;
}
void LineShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	if (cSelectedPointIndex < 0 || !IsCompleted()) return;
	cRenderCoordinates->at(cSelectedPointIndex) = mousePoint;
}
void PolygonShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	if (cSelectedPointIndex < 0) return;
	cRenderCoordinates->outer().at(cSelectedPointIndex) = mousePoint;
	if (IsCompleted()) {
		if (cSelectedPointIndex == 0)cRenderCoordinates->outer().back() = mousePoint;
		if (cSelectedPointIndex == GetNumberOfPoints() - 1)cRenderCoordinates->outer().front() = mousePoint;
	}
	Tesselate();
}
// GetWKT ----------------------------------------------------------------
bool PointShape::GetWKT(std::string& wkt) {
	if (!cIsComplete) return false;
	wkt = boost::geometry::to_wkt(cRenderCoordinate);
	return true;
}
bool LineShape::GetWKT(std::string& wkt) {
	if (!IsCompleted()) return false;
	wkt = boost::geometry::to_wkt(*cRenderCoordinates.get());
	return true;
}
bool PolygonShape::GetWKT(std::string& wkt) {
	if (!IsCompleted()) return false;
	wkt = boost::geometry::to_wkt(*cRenderCoordinates.get());
	return true;
}


// Other -----------------------------------------------------------------------------

void PolygonShape::SetDrawMode(bool bPolygon, bool bLines, bool bPoints) {
	cbDrawPolygon = bPolygon;
	cbDrawLines = bLines;
	cbDrawPoints = bPoints;
}

ShapeType Shape::GetType() { return cType; }

wxColour Shape::GetColor() { return cColor; }

void Shape::Finish() { cFinished = true; }