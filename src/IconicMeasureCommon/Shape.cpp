#include <IconicMeasureCommon/Shape.h>
#include <IconicMeasureCommon/Geometry.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace iconic;

// Constructors ---------------------------------------------------------------------------
Shape::Shape(ShapeType t, wxColour c) {
	type = t;
	color = c;
	selectedPointIndex = -1;
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

LineShape::LineShape(wxColour c) : Shape(ShapeType::LineType, c) {
	renderCoordinates = Geometry::VectorTrainPtr(new Geometry::VectorTrain);
	coordinates = Geometry::VectorTrain3DPtr(new Geometry::VectorTrain3D);
}
PolygonShape::PolygonShape(wxColour c) : Shape(ShapeType::PolygonType, c) {
	renderCoordinates = Geometry::PolygonPtr(new Geometry::Polygon);
	coordinates = Geometry::Polygon3DPtr(new Geometry::Polygon3D);
}
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
boost::shared_ptr<HeightProfile> PointShape::GetHeightProfile() {
	return NULL;
}
boost::shared_ptr<HeightProfile> LineShape::GetHeightProfile() {
	return profile;
}
boost::shared_ptr<HeightProfile> PolygonShape::GetHeightProfile() {
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

	// Add the first point again to the end of the polygon as you can above the first and last point otherwise, does not seem to be treated as closed
	this->renderCoordinates->outer().push_back(this->renderCoordinates->outer()[0]);

	if (boost::geometry::within(mouseClick, *renderCoordinates.get())) {
		this->renderCoordinates->outer().pop_back();
		return true;
	}
	else {
		this->renderCoordinates->outer().pop_back();
		return false;
	}
}
// GetPoint -------------------------------------------------------------
Geometry::PointPtr PointShape::GetPoint(Geometry::Point mouseClick) {
	// This should occur by selecting the entire shape instead
	return NULL;
}
Geometry::PointPtr LineShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : *renderCoordinates) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			return Geometry::PointPtr(&p);
		}
		i++;
	}
	//https://gis.stackexchange.com/questions/127783/distance-between-a-point-and-linestring-postgis-geos-vs-boost
	return NULL;
}
Geometry::PointPtr PolygonShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Geometry::Point& p : renderCoordinates.get()->outer()) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			return Geometry::PointPtr(&p);
		}
		i++;
	}
	/*
	* If a created point has not been created, a new point could be created here
	*/
	return NULL;
}
// GetRenderingPoint ---------------------------------------------------
Geometry::Point PointShape::GetRenderingPoint(int index) {
	return renderCoordinate;
}
Geometry::Point LineShape::GetRenderingPoint(int index) {
	if (index == -1)
		return renderCoordinates->back();
	else
		return renderCoordinates->at(index);
}
Geometry::Point PolygonShape::GetRenderingPoint(int index) {
	if (index == -1)
		return renderCoordinates->outer().back();
	else
		return renderCoordinates->outer().at(index);
}
// AddPoint ------------------------------------------------------------
bool PointShape::AddPoint(Geometry::Point newPoint, int index) {
	// Adding a point only occurs when defining the point
	if (!isComplete) {
		renderCoordinate = newPoint;
		isComplete = true;
		// UpdateCalculations should be called after the point has been defined
		return true;
	}
	return false;
}
bool LineShape::AddPoint(Geometry::Point newPoint, int index) {
	if (index == -1)
		renderCoordinates->push_back(newPoint);
	else
		renderCoordinates->insert(renderCoordinates->begin() + index, newPoint);

	return true;
}
bool PolygonShape::AddPoint(Geometry::Point newPoint, int index) {
	if (index == -1)
		renderCoordinates->outer().push_back(newPoint);
	else
		renderCoordinates->outer().insert(renderCoordinates->outer().begin() + index, newPoint);

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
	if (this->renderCoordinates->size() <= 1) return;
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

	length = boost::geometry::length(*renderCoordinates);

	Geometry::Point3D start, end;
	Geometry::Point3D differenceVec;
	std::vector<double> zValues;
	double norm;

	for (int i = 0; i < coordinates->size() - 1; i++) {
		start = coordinates->at(i);
		end = coordinates->at(i + 1);

		differenceVec.set<0>(end.get<0>() - start.get<0>());
		differenceVec.set<1>(end.get<1>() - start.get<1>());
		differenceVec.set<2>(end.get<2>() - start.get<2>());

		norm = boost::geometry::distance(start, end);

		wxLogVerbose(_("Norm " + std::to_string(i) + " : " + std::to_string(norm)));

		/*
		The code below is not implemented correctly since the scale of the norm is based on the object coordinates
		The sampling will at most take two points which is not great
		This should be fixed after discussions with I-CONIC
		-
		Alex
		*/

		//differenceVec.set<0>(differenceVec.get<0>() / norm);
		//differenceVec.set<1>(differenceVec.get<1>() / norm);
		if (norm != 0)
			differenceVec.set<2>(differenceVec.get<2>() / norm);
		else
			norm = 0.0001;
		for (int j = 0; j < norm; j++) {
			zValues.push_back(start.get<2>() + differenceVec.get<2>() * j);
		}

	}
}
void PolygonShape::UpdateCalculations(Geometry& g) {
	for (int i = 0; i < coordinates->outer().size(); i++) {
		if (!g.ImageToObject(this->renderCoordinates->outer().at(i), this->coordinates->outer().at(i)))
		{
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
	}

	length = boost::geometry::length(renderCoordinates->outer());
	area = boost::geometry::area(renderCoordinates->outer());
	volume = area * 5; // Not a correct solution
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





ShapeType Shape::GetType() { return this->type; }

wxColour Shape::GetColor() { return this->color; }
bool Shape::HasPanel() {return this->panel != nullptr;}
void Shape::SetPanel(wxPanel* panel) { this->panel = panel; }
wxPanel* Shape::GetPanel() { return this->panel; }