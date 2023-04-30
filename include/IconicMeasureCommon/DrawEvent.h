#pragma once
#include    <wx/event.h>
#include <IconicMeasureCommon/exports.h>
#include <IconicMeasureCommon/Geometry.h>


/**
 * @brief This event is raised in the ImageCanvas::OnPaint method to request the shapes to be drawn
*/
class ICONIC_MEASURE_COMMON_EXPORT DrawEvent : public wxCommandEvent
{
public:
	/**
	 * @brief Constructor
	 * @param eventType Accepts DRAW_SHAPES
	 * @param winid Window id of sending event handler
	 * @param x X coordinate (normalized camera coordinates)
	 * @param y Y coordinate (normalized camera coordinates)
	 * @param measure Says if the program is in the measure-mode or not
	*/
	DrawEvent(wxEventType eventType, int winid, const float& x, const float& y, const bool measure);
	/**
	 * @brief Get the point coordinates.
	 * 
	 * Normalized camera coordinates.
	 * @param x X coordinate
	 * @param y Y coordinate
	*/
	void GetPoint(float& x, float& y) const;
	/**
	 * @brief Get if the program is in the measure mode
	 * @return True if the program is in the measure mode, false otherwise
	*/
	bool IsMeasuring() const;

private:
	const float x;
	const float y;
	const bool isMeasuring;
};

wxDECLARE_EVENT(DRAW_SHAPES, DrawEvent);