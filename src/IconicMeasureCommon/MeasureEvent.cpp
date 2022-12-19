#include	<IconicMeasureCommon/MeasureEvent.h>

wxDEFINE_EVENT(MEASURE_POINT, MeasureEvent);

MeasureEvent::MeasureEvent(wxEventType eventType, int winid, const float& x, const float& y, const EAction action)
    : wxCommandEvent(eventType, winid),
    m_x(x),
    m_y(y),
    m_action(action)
{
}

void MeasureEvent::GetPoint(float& x, float& y) const 
{ 
    x = m_x; 
    y = m_y; 
}

wxEvent* MeasureEvent::Clone() const 
{ 
    return new MeasureEvent(*this); 
}

MeasureEvent::EAction MeasureEvent::GetAction() const {
    return m_action;
}
