#pragma once
#include    <wx/event.h>
#include <IconicMeasureCommon/exports.h>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/Shape.h>

/**
 * @brief An event that holds data regarding a shape that should be presented.
 *
 * E.g. issued from MeasureHandler when a shape is updated or edited
*/
class ICONIC_MEASURE_COMMON_EXPORT DataUpdateEvent : public wxCommandEvent
{
public:
	/**
	 * @brief General constructor for modifying data presentation
	 * @param winid Window id
	*/
	DataUpdateEvent(int winid);
	/**
	 * @brief Constructor specifically for when deleting a shape
	 * @param winid Window id
	 * @param index Index of shape to delete
	*/
	DataUpdateEvent(int winid, int index);
	/**
	 * @brief Initializer for updating data for a point
	 * @param index The index of the shape
	 * @param p The coordinates of the point
	*/
	void Initialize(const int index, const iconic::ShapePtr shape);

	/**
	 * @brief Returns the associated shape
	 * @return The pointer to the shape
	*/
	iconic::ShapePtr GetShape();

	/**
	 * @brief Get the point coordinates.
	 *
	 * Object coordinates
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param z Z coordinate
	*/
	void GetPoint(float& x, float& y, float& z) const;

	/**
	 * @brief Get the shape index
	 * @return The shape index
	*/
	int GetIndex() const;
	
	/**
	 * @brief Says if the event notifies that a shape has been deleted
	 * @return True if it is a delete event, false otherwise
	*/
	bool IsDeletionEvent() const;


	/**
	 * @brief Deep copy of parameters
	 * @return Cloned copy
	*/
	virtual wxEvent* Clone() const;
private:
	int cShapeIndex;
	iconic::ShapePtr cpShape;

	bool cDeleteEvent;
};

wxDECLARE_EXPORTED_EVENT(ICONIC_MEASURE_COMMON_EXPORT, DATA_UPDATE, DataUpdateEvent);

