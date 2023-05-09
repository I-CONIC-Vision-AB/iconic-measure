#include <IconicMeasureCommon/DataUpdateEvent.h>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/Shape.h>

wxDEFINE_EVENT(DATA_UPDATE, DataUpdateEvent);

DataUpdateEvent::DataUpdateEvent(int winid)
	: wxCommandEvent(DATA_UPDATE, winid) {
	cDeleteEvent = false;
}

DataUpdateEvent::DataUpdateEvent(int winid, int index)
	: wxCommandEvent(DATA_UPDATE, winid) {
	cShapeIndex = index;
	cDeleteEvent = true;
}

void DataUpdateEvent::Initialize(const int index, const iconic::ShapePtr shape) {
	cShapeIndex = index;
	cpShape = shape;
}


iconic::ShapePtr DataUpdateEvent::GetShape() { return cpShape; }


int DataUpdateEvent::GetIndex() const { return cShapeIndex; }
bool DataUpdateEvent::IsDeletionEvent() const { return cDeleteEvent; }



wxEvent* DataUpdateEvent::Clone() const {
	return new DataUpdateEvent(*this);
}

