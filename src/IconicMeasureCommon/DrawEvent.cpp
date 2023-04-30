#include <IconicMeasureCommon/DrawEvent.h>


wxDEFINE_EVENT(DRAW_SHAPES, DrawEvent);

DrawEvent::DrawEvent(wxEventType eventType, int winid, const float& x, const float& y, const bool measure)
	: wxCommandEvent(eventType, winid),
	x(x),
	y(y),
	isMeasuring(measure)
{
}

void DrawEvent::GetPoint(float& x, float& y) const
{
	x = this->x;
	y = this->y;
}

bool DrawEvent::IsMeasuring() const
{
	return isMeasuring;
}


