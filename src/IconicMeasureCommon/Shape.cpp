#include <IconicMeasureCommon/Geometry.h>

using namespace iconic;

// Constructors ---------------------------------------------------------------------------
Geometry::Shape::Shape(ShapeType t, wxColour c) {
	type = t;
	color = c;
	selectedPointIndex = -1;
	panel = nullptr;
};

Geometry::PointShape::PointShape(wxColour c) : Geometry::Shape(Geometry::ShapeType::PointType, c) {
	renderCoordinate = Point(-1, -1);
	coordinate = Point3D(-1, -1, -1);
	isComplete = false;
}

Geometry::LineShape::LineShape(wxColour c) : Geometry::Shape(Geometry::ShapeType::LineType, c) {
	renderCoordinates = VectorTrainPtr(new VectorTrain);
	coordinates = VectorTrain3DPtr(new VectorTrain3D);
}
Geometry::PolygonShape::PolygonShape(wxColour c) : Shape(ShapeType::PolygonType, c) {
	renderCoordinates = PolygonPtr(new Polygon);
	coordinates = Polygon3DPtr(new Polygon3D);
}
// GetArea ------------------------------------------------------------------------------
double Geometry::PointShape::GetArea() {
	return -1;
}
double Geometry::LineShape::GetArea() {
	return -1;
}
double Geometry::PolygonShape::GetArea() {
	return area;
}
// GetLength -------------------------------------------------------------
double Geometry::PointShape::GetLength() {
	return -1;
}
double Geometry::LineShape::GetLength() {
	return length;
}
double Geometry::PolygonShape::GetLength() {
	return length;
}
// GetVolume -------------------------------------------------------------
double Geometry::PointShape::GetVolume() {
	return -1;
}
double Geometry::LineShape::GetVolume() {
	return -1;
}
double Geometry::PolygonShape::GetVolume() {
	return volume;
}
//GetHeightProfile ---------------------------------------------------
boost::shared_ptr<Geometry::HeightProfile> Geometry::PointShape::GetHeightProfile() {
	return NULL;
}
boost::shared_ptr<Geometry::HeightProfile> Geometry::LineShape::GetHeightProfile() {
	return profile;
}
boost::shared_ptr<Geometry::HeightProfile> Geometry::PolygonShape::GetHeightProfile() {
	return NULL;
}
// Select -------------------------------------------------------------
bool Geometry::PointShape::Select(Geometry::Point mouseClick) {
	// Add ImageCanvas::GetScale() as argument?
	if (boost::geometry::distance(mouseClick, renderCoordinate) < 0.005f) { // Should depend on the zoom amount
		return true;
	}
	else {
		return false;
	}
}
bool Geometry::LineShape::Select(Geometry::Point mouseClick) {
	if (boost::geometry::distance(mouseClick, *renderCoordinates.get()) < 0.001f) { // Should depend on the zoom amount
		return true;
	}
	else {
		return false;
	}
}
bool Geometry::PolygonShape::Select(Geometry::Point mouseClick) {

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
Geometry::PointPtr Geometry::PointShape::GetPoint(Geometry::Point mouseClick) {
	// This should occur by selecting the entire shape instead
	return NULL;
}
Geometry::PointPtr Geometry::LineShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Point& p : *renderCoordinates) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			return PointPtr(&p);
		}
		i++;
	}
	//https://gis.stackexchange.com/questions/127783/distance-between-a-point-and-linestring-postgis-geos-vs-boost
	return NULL;
}
Geometry::PointPtr Geometry::PolygonShape::GetPoint(Geometry::Point mouseClick) {
	int i = 0;
	for (Point& p : renderCoordinates.get()->outer()) {
		if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
			selectedPointIndex = i;
			return PointPtr(&p);
		}
		i++;
	}
	/*
	* If a created point has not been created, a new point could be created here
	*/
	return NULL;
}
// GetRenderingPoint ---------------------------------------------------
Geometry::Point Geometry::PointShape::GetRenderingPoint(int index) {
	return renderCoordinate;
}
Geometry::Point Geometry::LineShape::GetRenderingPoint(int index) {
	if (index == -1)
		return renderCoordinates->back();
	else
		return renderCoordinates->at(index);
}
Geometry::Point Geometry::PolygonShape::GetRenderingPoint(int index) {
	if (index == -1)
		return renderCoordinates->outer().back();
	else
		return renderCoordinates->outer().at(index);
}
// AddPoint ------------------------------------------------------------
bool Geometry::PointShape::AddPoint(Geometry::Point newPoint, int index) {
	// Adding a point only occurs when defining the point
	if (!isComplete) {
		renderCoordinate = newPoint;
		isComplete = true;
		// UpdateCalculations should be called after the point has been defined
		return true;
	}
	return false;
}
bool Geometry::LineShape::AddPoint(Geometry::Point newPoint, int index) {
	if (index == -1)
		renderCoordinates->push_back(newPoint);
	else
		renderCoordinates->insert(renderCoordinates->begin() + index, newPoint);

	return true;
}
bool Geometry::PolygonShape::AddPoint(Geometry::Point newPoint, int index) {
	if (index == -1)
		renderCoordinates->outer().push_back(newPoint);
	else
		renderCoordinates->outer().insert(renderCoordinates->outer().begin() + index, newPoint);

	return true;
}
//UpdateCalculations -----------------------------------------------------------
void Geometry::PointShape::UpdateCalculations(Geometry& g) {
	if (!isComplete) return;
	if (!g.ImageToObject(this->renderCoordinate, this->coordinate))
	{
		wxLogError(_("Could not compute image-to-object coordinates for measured point"));
		return;
	}
}
void Geometry::LineShape::UpdateCalculations(Geometry& g) {
	if (this->renderCoordinates->size() <= 1) return;
	this->coordinates->clear();
	Point3D p;
	for (int i = 0; i < renderCoordinates->size(); i++) {
		if (!g.ImageToObject(this->renderCoordinates->at(i), p))
		{
			wxLogError(_("Could not compute image-to-object coordinates for measured point"));
			return;
		}
		this->coordinates->push_back(p);
	}

	length = boost::geometry::length(*renderCoordinates);

	Point3D start, end;
	Point3D differenceVec;
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
void Geometry::PolygonShape::UpdateCalculations(Geometry& g) {
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
int Geometry::PointShape::GetNumberOfPoints() {
	return 1;
}
int Geometry::LineShape::GetNumberOfPoints() {
	return renderCoordinates->size();
}
int Geometry::PolygonShape::GetNumberOfPoints() {
	return renderCoordinates->outer().size();
}

// IsCompleted -------------------------------------------------------------
bool Geometry::PointShape::IsCompleted() {
	return isComplete;
}
bool Geometry::LineShape::IsCompleted() {
	return renderCoordinates->size() > 1;
}
bool Geometry::PolygonShape::IsCompleted() {
	return renderCoordinates->outer().size() > 2;
}



Geometry::ShapeType Geometry::Shape::GetType() { return this->type; }

wxColour Geometry::Shape::GetColor() { return this->color; }