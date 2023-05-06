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
	DataUpdateEvent(int winid);
	/**
	 * @brief Initializer for updating data for a point
	 * @param index The index of the shape
	 * @param p The coordinates of the point
	*/
	void Initialize(const int index, const iconic::Geometry::Point3D p, const wxColour color);
	/**
	 * @brief Initializer for updating data for a vectortrain
	 * @param index The index of the shape
	 * @param length The length of the line
	 * @param profile The heightproifle of the line
	*/
	void Initialize(const int index, const double length, const boost::shared_ptr<iconic::Geometry::HeightProfile> profile, const wxColour color);
	/**
	 * @brief Initializer for updating data for a polygon
	 * @param index The index of the shape
	 * @param length The perimeter length of the polygon
	 * @param area The area of the polygon
	 * @param volume The volume of the polygon
	*/
	void Initialize(const int index, const double length, const double area, const double volume, const wxColour color);

	/**
	 * @brief Initializer for deleting data of a shape
	 * @param index The index of the shape
	 * @param deleteShape Indicates if shape data should be removed
	*/
	void Initialize(const int index, const bool deleteShape);

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
	 * @brief Gets the length value
	 * @return The length value
	*/
	double GetLength() const;
	/**
	 * @brief Gets the area value
	 * @return The area value
	*/
	double GetArea() const;
	/**
	 * @brief Gets the volume value
	 * @return The volume value
	*/
	double GetVolume() const;
	/**
	 * @brief Get the heightprofile data
	 * @return The heightprofile
	*/
	boost::shared_ptr<iconic::Geometry::HeightProfile> GetHeightProfile() const;
	/**
	 * @brief Get the shape index
	 * @return The shape index
	*/
	int GetIndex() const;
	/**
	 * @brief Get the shape type
	 * @return The shape type
	*/
	iconic::ShapeType GetShapeType() const;
	/**
	 * @brief Get the shape color
	 * @return The shape color
	*/
	wxColor GetShapeColor() const;


	/**
	 * @brief Deep copy of parameters
	 * @return Cloned copy
	*/
	virtual wxEvent* Clone() const;
private:
	int cShapeIndex;
	iconic::Geometry::Point3D cPoint;
	double cLength;
	double cArea;
	double cVolume;
	boost::shared_ptr<iconic::Geometry::HeightProfile> cProfile;
	iconic::ShapeType cType;
	wxColor cColor;
};

wxDECLARE_EVENT(DATA_UPDATE, DataUpdateEvent);
wxDECLARE_EVENT(DATA_REMOVE, DataUpdateEvent);
