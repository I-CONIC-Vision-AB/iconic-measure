#pragma once
#include    <wx/event.h>
#include <IconicMeasureCommon/exports.h>

/**
 * @brief An event that holds a point and an action.
 *
 * E.g. issued from ImageCanvas when in measure mode and mouse left clicked
*/
class ICONIC_MEASURE_COMMON_EXPORT MeasureEvent : public wxCommandEvent
{
public:
	/**
	 * @brief Type of measure event
	*/
	enum class EAction {
		ADDED,      //!< Point was added
		DELETED,    //!< Point was deleted
		SELECTED,   //!< Point was selected
		FINISHED,	//!< Measure was finished
		MOVED,		//!< Mouse moved while measuring
		SELECT		//!< Select a shape
	};

	/**
	 * @brief Constructor
	 * @param eventType Currently always MEASURE_POINT, but may include other types later
	 * @param winid Window id of sending event handler
	 * @param x X coordinate (normalized camera coordinates)
	 * @param y Y coordinate (normalized camera coordinates)
	 * @param action
	*/
	MeasureEvent(wxEventType eventType, int winid, const float& x, const float& y, const EAction action);

	/**
	 * @brief Get the point coordinates.
	 *
	 * Normalized camera coordinates
	 * @param x X coordinate
	 * @param y Y coordinate
	*/
	void GetPoint(float& x, float& y) const;

	/**
	 * @brief Get kind of measure event
	 * @return kind of measure event
	*/
	EAction GetAction() const;

	/**
	 * @brief Deep copy of parameters
	 * @return Cloned copy
	*/
	virtual wxEvent* Clone() const;
private:
	const float m_x;
	const float m_y;
	const EAction m_action;
};

wxDECLARE_EVENT(MEASURE_POINT, MeasureEvent);
