#include <IconicMeasureCommon/DrawEvent.h>


wxDEFINE_EVENT(DRAW_SHAPES, DrawEvent);

DrawEvent::DrawEvent(wxEventType eventType, int winid, const float& x, const float& y, const bool measure)
	: wxCommandEvent(eventType, winid),
	m_x(x),
	m_y(y),
	cIsMeasuring(measure) {}

void DrawEvent::GetPoint(float& x, float& y) const {
	x = m_x;
	y = m_y;
}

bool DrawEvent::IsMeasuring() const {
	return cIsMeasuring;
}


