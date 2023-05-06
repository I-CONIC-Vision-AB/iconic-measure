#include <IconicMeasureCommon/DataUpdateEvent.h>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/Shape.h>

wxDEFINE_EVENT(DATA_UPDATE, DataUpdateEvent);
wxDEFINE_EVENT(DATA_REMOVE, DataUpdateEvent);

DataUpdateEvent::DataUpdateEvent(int winid)
	: wxCommandEvent(DATA_UPDATE, winid)
{
	cDeleteEvent = false;
}

DataUpdateEvent::DataUpdateEvent(int winid, int index)
	: wxCommandEvent(DATA_UPDATE, winid)
{
	cShapeIndex = index;
	cDeleteEvent = true;
}

void DataUpdateEvent::Initialize(const int index, const iconic::Geometry::Point3D p, const wxColour color)
{
	cShapeIndex = index;
	cColor = color;
	cPoint = p;
	cType = iconic::ShapeType::PointType;
}
void DataUpdateEvent::Initialize(const int index, const double length, const boost::shared_ptr<iconic::Geometry::HeightProfile> profile, const wxColour color)
{
	cShapeIndex = index;
	cColor = color;
	cLength = length;
	cProfile = profile;
	cType = iconic::ShapeType::LineType;
}
void DataUpdateEvent::Initialize(const int index, const double length, const double area, const double volume, const wxColour color)
{
	cShapeIndex = index;
	cColor = color;
	cLength = length;
	cArea = area;
	cVolume = volume;
	cType = iconic::ShapeType::PolygonType;
}

void DataUpdateEvent::GetPoint(float& x, float& y, float& z) const
{
	x = cPoint.get<0>();
	y = cPoint.get<1>();
	z = cPoint.get<2>();
}
boost::shared_ptr<iconic::Geometry::HeightProfile> DataUpdateEvent::GetHeightProfile() const {
	return cProfile;
}

double DataUpdateEvent::GetLength() const { return cLength; }
double DataUpdateEvent::GetArea() const { return cArea; }
double DataUpdateEvent::GetVolume() const { return cVolume; }
int DataUpdateEvent::GetIndex() const { return cShapeIndex; }
iconic::ShapeType DataUpdateEvent::GetShapeType() const { return cType; }
wxColor DataUpdateEvent::GetShapeColor() const { return cColor; }
bool DataUpdateEvent::IsDeletionEvent() const { return cDeleteEvent; }



wxEvent* DataUpdateEvent::Clone() const
{
	return new DataUpdateEvent(*this);
}

