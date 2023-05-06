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
	type = t;
	color = c;
	selectedPointIndex = -1;
	nextInsertIndex = 0;
	panel = nullptr;
};

Shape::~Shape() {
	if (panel != nullptr) {
		panel->Destroy();
	}
}

PointShape::PointShape(wxColour c) : Shape(ShapeType::PointType, c) {
	renderCoordinate = Geometry::Point(-1, -1);
	coordinate = Geometry::Point3D(-1, -1, -1);
	isComplete = false;
}
PointShape::~PointShape() {}

LineShape::LineShape(wxColour c) : Shape(ShapeType::LineType, c) {
	renderCoordinates = Geometry::VectorTrainPtr(new Geometry::VectorTrain);
	coordinates = Geometry::VectorTrain3DPtr(new Geometry::VectorTrain3D);
	profile = boost::shared_ptr<Geometry::HeightProfile>(new Geometry::HeightProfile());
}
LineShape::~LineShape() {}

PolygonShape::PolygonShape(wxColour c) : 
	Shape(ShapeType::PolygonType, c), 
	cpTesselator(nullptr),
	renderCoordinates(new Geometry::Polygon),
	coordinates(new Geometry::Polygon3D)
{
	SetDrawMode();
}

PolygonShape::PolygonShape(Geometry::PolygonPtr pPolygon, wxColour c) : Shape(ShapeType::PolygonType, c),
cpTesselator(nullptr) {
	renderCoordinates = pPolygon;
	coordinates = Geometry::Polygon3DPtr(new Geometry::Polygon3D);
	SetDrawMode();
	if (renderCoordinates) {
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
void PointShape::GetCoordinate(Geometry::Point3D& coordinate){
	coordinate.set<0>(this->coordinate.get<0>());
	coordinate.set<1>(this->coordinate.get<1>());
	coordinate.set<2>(this->coordinate.get<2>());
}
void LineShape::GetCoordinate(Geometry::Point3D& coordinate){}
void PolygonShape::GetCoordinate(Geometry::Point3D& coordinate){}

// GetArea ------------------------------------------------------------------------------
double PointShape::GetArea() {
	return -1;
}
double LineShape::GetArea() {
	return -1;
}
double PolygonShape::GetArea() {
	return area;
}
// GetLength -------------------------------------------------------------
double PointShape::GetLength() {
	return -1;
}
double LineShape::GetLength() {
	return length;
}
double PolygonShape::GetLength() {
	return length;
}
// GetVolume -------------------------------------------------------------
double PointShape::GetVolume() {
	return -1;
}
double LineShape::GetVolume() {
	return -1;
}
double PolygonShape::GetVolume() {
	return volume;
}
//GetHeightProfile ---------------------------------------------------
boost::shared_ptr<Geometry::HeightProfile> PointShape::GetHeightProfile() {
	return NULL;
}
boost::shared_ptr<Geometry::HeightProfile> LineShape::GetHeightProfile() {
	return profile;
}
boost::shared_ptr<Geometry::HeightProfile> PolygonShape::GetHeightProfile() {
	return NULL;
}
// Select -------------------------------------------------------------
bool PointShape::Select(Geometry::Point mouseClick) {
	// Add ImageCanvas::GetScale() as argument?
	if (boost::geometry::distance(mouseClick, renderCoordinate) < 0.005f) { // Should depend on the zoom amount
		return true;
	}
	else {
		return false;
	}
}
bool LineShape::Select(Geometry::Point mouseClick) {
	if (boost::geometry::distance(mouseClick, *renderCoordinates.get()) < 0.001f) { // Should depend on the zoom amount
		return true;
	}
	else {
		return false;
	}
}
bool PolygonShape::Select(Geometry::Point mouseClick) {
	return boost::geometry::within(mouseClick, *renderCoordinates.get());
}
// GetPoint -------------------------------------------------------------
bool PointShape::GetPoint(Geometry::Point mouseClick) {
	return boost::geometry::distance(mouseClick, this->renderCoordinate) < 0.005f;
}
bool LineShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : *renderCoordinates) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			return true;
		}
		i++;
	}
	//https://gis.stackexchange.com/questions/127783/distance-between-a-point-and-linestring-postgis-geos-vs-boost
	return false;
}
bool PolygonShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : renderCoordinates.get()->outer()) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			wxLogVerbose(_("Selected index: " + std::to_string(selectedPointIndex)));
			return true;
		}
		i++;
	}
	return false;
}
// GetRenderingPoint ---------------------------------------------------
Geometry::Point PointShape::GetRenderingPoint(int index) {
	return renderCoordinate;
}
Geometry::Point LineShape::GetRenderingPoint(int index) {
	if (index == -1 || index >= this->GetNumberOfPoints())
		return renderCoordinates->back();
	else
		return renderCoordinates->at(index);
}
Geometry::Point PolygonShape::GetRenderingPoint(int index) {
	if (index >= this->GetNumberOfPoints())
		return renderCoordinates->outer().at(0);
	if (index != 0 && !this->IsCompleted())
		return renderCoordinates->outer().back();
	else if (index == -1)
		return renderCoordinates->outer().at(this->GetNumberOfPoints() - 2); // Skip the last point that is identical to the first
	else
		return renderCoordinates->outer().at(index);
}
// Draw ---------------------------------------------------------------
void PointShape::Draw(bool selected, Geometry::Point mousePoint) {
	glPushAttrib(GL_CURRENT_BIT);	// Apply color until pop
	glColor3ub(this->GetColor().Red(), this->GetColor().Green(), this->GetColor().Blue());		  // Color of geometry
	(selected) ? glPointSize(20.f) : glPointSize(10.f);
	glBegin(GL_POINTS);
	glVertex2f(this->renderCoordinate.get<0>(), this->renderCoordinate.get<1>());
	glEnd();
	glPopAttrib();
}
void LineShape::Draw(bool selected, Geometry::Point mousePoint) {
	wxColour color = this->GetColor();
	// Draw the measured points
	glPushAttrib(GL_CURRENT_BIT); // Apply color until pop
	glColor3ub(color.Red(), color.Green(), color.Blue());		  // Color of geometry
	glLineWidth(3.f);
	glBegin(GL_LINE_STRIP);

	Geometry::Point p;
	for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
	{
		p = this->GetRenderingPoint(i);
		glVertex2f(p.get<0>(), p.get<1>());
	}
	glEnd();

	if (selected) {
		glPointSize(10.f);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
		{
			p = this->GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();


		// Draw the mouse track
		glBegin(GL_LINE_LOOP);
		this->GetPossibleIndex(mousePoint);
		if (this->GetPossibleIndex(mousePoint) == 0) {
			glVertex2f(this->GetRenderingPoint(0).get<0>(), this->GetRenderingPoint(0).get<1>());
			glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());
		}
		else {
			glVertex2f(this->GetRenderingPoint(nextInsertIndex-1).get<0>(), this->GetRenderingPoint(nextInsertIndex - 1).get<1>());
			glVertex2f(this->GetRenderingPoint(nextInsertIndex).get<0>(), this->GetRenderingPoint(nextInsertIndex).get<1>());
			glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());
		}

		glEnd();
	}

	glPopAttrib();
}
void PolygonShape::Draw(bool selected, Geometry::Point mousePoint) {
	glPushAttrib(GL_CURRENT_BIT);	// Apply color until pop
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw mouse track
	if (selected) {
		glColor3ub(color.Red(), color.Green(), color.Blue());
		this->GetPossibleIndex(mousePoint);
		glColor3ub(color.Red(), color.Green(), color.Blue());			// Color of geometry
		glLineWidth(3.f);
		glBegin(GL_LINE_LOOP);


		glVertex2f(this->GetRenderingPoint(nextInsertIndex - 1).get<0>(), this->GetRenderingPoint(nextInsertIndex - 1).get<1>());
		glVertex2f(this->GetRenderingPoint(nextInsertIndex).get<0>(), this->GetRenderingPoint(nextInsertIndex).get<1>());
		glVertex2f(mousePoint.get<0>(), mousePoint.get<1>());

		glEnd();
	}

	if (!IsCompleted()) {
		glColor3ub(color.Red(), color.Green(), color.Blue());
		glLineWidth(3.f);
		glPointSize(10.f);
		Geometry::Point p;
		
		glBegin(GL_LINE_STRIP);
		
		for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
		{
			p = this->GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
		glBegin(GL_POINTS);
		for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
		{
			p = this->GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
		glPopAttrib();
		return;
	}
	if (!cpTesselator) this->Tesselate();

	// Get tesselated pieces.
	const float* verts = tessGetVertices(cpTesselator);
	const int* elems = tessGetElements(cpTesselator);
	const int nverts = tessGetVertexCount(cpTesselator);
	const int nelems = tessGetElementCount(cpTesselator);

	int i, j;
	const int triangles = 3;
	if (cbDrawPolygon && !selected) {
		// Draw polygons.
		glColor4ub(color.Red(), color.Green(), color.Blue(), selected ? color.Alpha() : color.Alpha()/2);
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
		glColor3ub(color.Red(), color.Green(), color.Blue());
		glLineWidth(3.f);
		glBegin(GL_LINE_LOOP);

		Geometry::Point p;
		for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
		{
			p = this->GetRenderingPoint(i);
			glVertex2f(p.get<0>(), p.get<1>());
		}
		glEnd();
	}

	if (cbDrawPoints || selected) {
		glColor3ub(color.Red(), color.Green(), color.Blue()); //glColor4ub(0, 255, 0, 255);
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
	if (this->isComplete) return this->GetPoint(newPoint); // If the point is completed the point should be selectable, but no new points can be added
	else {
		renderCoordinate = newPoint;
		isComplete = true;
		// UpdateCalculations should be called after the point has been defined
		return true;
	}
}
bool LineShape::AddPoint(Geometry::Point newPoint, int index) {
	if (this->GetPoint(newPoint)) return true;

	if (this->IsCompleted() && this->nextInsertIndex < this->GetNumberOfPoints()) {
		renderCoordinates->insert(renderCoordinates->begin() + this->nextInsertIndex, newPoint);
		this->selectedPointIndex = this->nextInsertIndex;
	}
	else {
		renderCoordinates->push_back(newPoint);
		this->selectedPointIndex = this->GetNumberOfPoints() - 1;
	}
	return true;
}
bool PolygonShape::AddPoint(Geometry::Point newPoint, int index) {
	if (this->IsCompleted() && this->GetPoint(newPoint)) { // See if a point could be selected before creating a new one
		return true; 
	}

	if (this->IsCompleted())
	{
		if (this->nextInsertIndex == 0) {
			renderCoordinates->outer().pop_back();
		}
			
		renderCoordinates->outer().insert(renderCoordinates->outer().begin() + this->nextInsertIndex, newPoint);
		this->selectedPointIndex = this->nextInsertIndex;
		if (this->nextInsertIndex == 0) {
			boost::geometry::correct(*(this->renderCoordinates));
		}
		this->Tesselate();
	}
	else {
		// Default if the polygon is not yet a polygon (i.e. has less than 3 points)
		renderCoordinates->outer().push_back(newPoint);
		this->selectedPointIndex = this->GetNumberOfPoints() - 1;
		if (this->IsCompleted())
			renderCoordinates->outer().push_back(renderCoordinates->outer().front());
	}

	return true;
}
//UpdateCalculations -----------------------------------------------------------
void PointShape::UpdateCalculations(Geometry& g) {
	if (!isComplete) return;
	if (!g.ImageToObject(this->renderCoordinate, this->coordinate))
	{
		wxLogError(_("Could not compute image-to-object coordinates for measured point"));
		return;
	}
}
void LineShape::UpdateCalculations(Geometry& g) {
	if (!this->IsCompleted()) return;
	this->coordinates->clear();
	Geometry::Point3D p;
	for (int i = 0; i < renderCoordinates->size(); i++) {
		if (!g.ImageToObject(this->renderCoordinates->at(i), p))
		{
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
		this->coordinates->push_back(p);
	}
	double currLen;
	length = 0;
	for (int i = 1; i < this->coordinates->size(); i++) {
		currLen = pow(coordinates->at(i).get<0>() - coordinates->at(i - 1).get<0>(), 2) + pow(coordinates->at(i).get<1>() - coordinates->at(i - 1).get<1>(), 2) + pow(coordinates->at(i).get<2>() - coordinates->at(i - 1).get<2>(), 2);
		length += sqrt(currLen);
	}

			// This is code meant for the heightprofile
	//Geometry::Point3D start, end;
	//Geometry::Point3D differenceVec;
	//std::vector<double> zValues;
	//double norm;

	//for (int i = 0; i < coordinates->size() - 1; i++) {
	//	start = coordinates->at(i);
	//	end = coordinates->at(i + 1);

	//	differenceVec.set<0>(end.get<0>() - start.get<0>());
	//	differenceVec.set<1>(end.get<1>() - start.get<1>());
	//	differenceVec.set<2>(end.get<2>() - start.get<2>());

	//	norm = boost::geometry::distance(start, end);

	//	wxLogVerbose(_("Norm " + std::to_string(i) + " : " + std::to_string(norm)));

	//	/*
	//	The code below is not implemented correctly since the scale of the norm is based on the object coordinates
	//	The sampling will at most take two points which is not great
	//	This should be fixed after discussions with I-CONIC
	//	-
	//	Alex
	//	*/

	//	//differenceVec.set<0>(differenceVec.get<0>() / norm);
	//	//differenceVec.set<1>(differenceVec.get<1>() / norm);
	//	if (norm != 0)
	//		differenceVec.set<2>(differenceVec.get<2>() / norm);
	//	else
	//		norm = 0.0001;
	//	for (int j = 0; j < norm; j++) {
	//		zValues.push_back(start.get<2>() + differenceVec.get<2>() * j);
	//	}

	//}
}
void PolygonShape::UpdateCalculations(Geometry& g) {
	boost::geometry::correct(*(this->renderCoordinates));
	/*Geometry::PolygonPtr newCoord = Geometry::PolygonPtr(new Geometry::Polygon);
	boost::geometry::convex_hull(*(this->renderCoordinates), *newCoord);
	this->renderCoordinates = newCoord;*/

	this->coordinates->clear();
	Geometry::Point3D objectPt;
	for (int i = 0; i < this->GetNumberOfPoints(); i++) {
		if (!g.ImageToObject(this->renderCoordinates->outer().at(i), objectPt))
		{
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
		this->coordinates->outer().push_back(objectPt);
	}
	//renderCoordinates->outer().push_back(renderCoordinates->outer().at(0));
	length = boost::geometry::perimeter(renderCoordinates->outer());
	//renderCoordinates->outer().pop_back();
	area = boost::geometry::area(renderCoordinates->outer());
	volume = area * 5; // Not a correct solution
}
void PolygonShape::Tesselate() {
	if (IsCompleted()) {
		if (cpTesselator) {
			tessDeleteTess(cpTesselator);
		}
		cpTesselator = tessNewTess(nullptr);

		// Tesselate exterior boundary
		tessSetOption(cpTesselator, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);
		size_t nPoints = renderCoordinates->outer().size();
		std::vector<float> vFloatPoints(2 * nPoints);
		for (int i = 0; i < nPoints; ++i) {
			const Geometry::Point& p = renderCoordinates->outer().at(i);
			vFloatPoints[2 * i] = p.get<0>();
			vFloatPoints[2 * i + 1] = p.get<1>();
		}
		tessAddContour(cpTesselator, 2, vFloatPoints.data(), sizeof(float) * 2, nPoints);

		// Tesselate holes in polygon if any
		for (int j = 0; j < renderCoordinates->inners().size(); ++j) {
			nPoints = renderCoordinates->inners()[j].size();
			if (vFloatPoints.size() < (2 * nPoints)) {
				vFloatPoints.resize(2 * nPoints);
			}
			for (int i = 0; i < nPoints; ++i) {
				const Geometry::Point& p = renderCoordinates->inners()[j].at(i);
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
	return renderCoordinates->size();
}
int PolygonShape::GetNumberOfPoints() {
	return renderCoordinates->outer().size();
}

// IsCompleted -------------------------------------------------------------
bool PointShape::IsCompleted() {
	return isComplete;
}
bool LineShape::IsCompleted() {
	return renderCoordinates->size() > 1;
}
bool PolygonShape::IsCompleted() {
	return renderCoordinates->outer().size() > 2;
}
// GetPossibleIndex -----------------------------------------------------------------------
int PointShape::GetPossibleIndex(Geometry::Point mousePoint) {
	return 0;
}
int LineShape::GetPossibleIndex(Geometry::Point mousePoint) {
	if (!this->IsCompleted()) return 0;
	int shortestIndex = 0;
	double shortestDistance = boost::geometry::distance(this->renderCoordinates->at(0), mousePoint);
	double currentDistance = 0;
	for (size_t i = 0; i < this->renderCoordinates->size(); i++)
	{
		currentDistance = boost::geometry::distance(this->renderCoordinates->at(i), mousePoint);
		if (currentDistance < shortestDistance) {
			shortestDistance = currentDistance;
			shortestIndex = i;
		}
	}

	double preDistance, postDistance = 0;
	preDistance = boost::geometry::distance(this->GetRenderingPoint(shortestIndex - 1), mousePoint); // GetRenderPoint loops back to the last element for index -1
	if (shortestIndex == 0) {
		Geometry::Point diff1(this->GetRenderingPoint(1).get<0>() - this->GetRenderingPoint(0).get<0>(), this->GetRenderingPoint(1).get<1>() - this->GetRenderingPoint(0).get<1>());
		Geometry::Point diff2(mousePoint.get<0>() - this->GetRenderingPoint(0).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(0).get<1>());
		if (boost::geometry::dot_product(diff1, diff2) > 0) return this->nextInsertIndex = shortestIndex + 1;
		else return this->nextInsertIndex = shortestIndex;
	}
	else if (shortestIndex == this->GetNumberOfPoints() - 1) {
		Geometry::Point diff1(this->GetRenderingPoint(shortestIndex - 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex - 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(mousePoint.get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff2) > 0) return this->nextInsertIndex = shortestIndex;
		else return this->nextInsertIndex = shortestIndex+1;
	}
	else {
		Geometry::Point diff1(this->GetRenderingPoint(shortestIndex - 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex - 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(this->GetRenderingPoint(shortestIndex + 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex + 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return this->nextInsertIndex = shortestIndex;
		else return this->nextInsertIndex = shortestIndex+1;
	}
}
int PolygonShape::GetPossibleIndex(Geometry::Point mousePoint) {
	if (!this->IsCompleted()) return 0;
	int shortestIndex = 0;
	double shortestDistance = boost::geometry::distance(this->renderCoordinates->outer().at(0), mousePoint);
	double currentDistance = 0;
	for (size_t i = 0; i < this->GetNumberOfPoints(); i++)
	{
		currentDistance = boost::geometry::distance(this->renderCoordinates->outer().at(i), mousePoint);
		if (currentDistance < shortestDistance) {
			shortestDistance = currentDistance;
			shortestIndex = i;
		}
	}
	double preDistance, postDistance = 0;
	preDistance = boost::geometry::distance(this->GetRenderingPoint(shortestIndex - 1), mousePoint); // GetRenderPoint loops back to the last element for index -1
	if (shortestIndex == 0) {
		Geometry::Point diff1(this->GetRenderingPoint(this->GetNumberOfPoints() - 2).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(this->GetNumberOfPoints() - 2).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(this->GetRenderingPoint(shortestIndex + 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex + 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return this->nextInsertIndex = shortestIndex;
		else return this->nextInsertIndex = shortestIndex + 1;

	}
	else if (shortestIndex == this->GetNumberOfPoints() - 1) {
		Geometry::Point diff1(this->GetRenderingPoint(shortestIndex - 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex - 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(this->GetRenderingPoint(1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return this->nextInsertIndex = shortestIndex;
		else return this->nextInsertIndex = shortestIndex + 1;
	}
	else {
		Geometry::Point diff1(this->GetRenderingPoint(shortestIndex - 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex - 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff2(this->GetRenderingPoint(shortestIndex + 1).get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), this->GetRenderingPoint(shortestIndex + 1).get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		Geometry::Point diff3(mousePoint.get<0>() - this->GetRenderingPoint(shortestIndex).get<0>(), mousePoint.get<1>() - this->GetRenderingPoint(shortestIndex).get<1>());
		if (boost::geometry::dot_product(diff1, diff3) > boost::geometry::dot_product(diff2, diff3)) return this->nextInsertIndex = shortestIndex;
		else return this->nextInsertIndex = shortestIndex + 1;
	}
}
// DeselectPoint --------------------------------------------------------------------
void PointShape::DeselectPoint() {
	isComplete = true;
}
void LineShape::DeselectPoint() {
	selectedPointIndex = -1;
}
void PolygonShape::DeselectPoint() {
	selectedPointIndex = -1;
}

// MoveSelectedPoint -----------------------------------------------------------------
void PointShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	this->renderCoordinate = mousePoint;
}
void LineShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	if (selectedPointIndex < 0 || !this->IsCompleted()) return;
	this->renderCoordinates->at(selectedPointIndex) = mousePoint;
}
void PolygonShape::MoveSelectedPoint(Geometry::Point mousePoint) {
	if (selectedPointIndex < 0) return;
	this->renderCoordinates->outer().at(selectedPointIndex) = mousePoint;
	if (this->IsCompleted()) {
		if (selectedPointIndex == 0)this->renderCoordinates->outer().back() = mousePoint;
		if (selectedPointIndex == this->GetNumberOfPoints() - 1)this->renderCoordinates->outer().front() = mousePoint;
	}
	this->Tesselate();
}


// Other -----------------------------------------------------------------------------

void PolygonShape::SetDrawMode(bool bPolygon, bool bLines, bool bPoints) {
	cbDrawPolygon = bPolygon;
	cbDrawLines = bLines;
	cbDrawPoints = bPoints;
}

ShapeType Shape::GetType() { return this->type; }

wxColour Shape::GetColor() { return this->color; }


bool Shape::HasPanel() {return this->panel != nullptr;}
void Shape::SetPanel(wxPanel* panel) { this->panel = panel; }
wxPanel* Shape::GetPanel() { return this->panel; }