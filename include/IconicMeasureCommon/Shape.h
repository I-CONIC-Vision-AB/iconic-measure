#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicMeasureCommon/Geometry.h>
#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <wx/wx.h>
#include <wx/colour.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

// Forward declaration
class TESStesselator;

namespace iconic {

	/**
	* @brief Secondary type to Shape that defines how the shape should be treated calculation and rendering wise
	*/
	enum ShapeType {
		None,			//!< Represents no shape
		PolygonType,	//!< A two-dimensional shape made up of multiple points
		LineType,		//!< A one-dimensional shape made up of multiple points
		PointType		//!< A single point
	};

	/**
	 * @brief The abstract base class for the Shapes
	 *
	 * Defines methods that a shape must implement to be considered a viable shape object.
	*/
	class ICONIC_MEASURE_COMMON_EXPORT Shape {
	public:
		/**
		 * @brief Retrieves the coordinate if the shape is a point. Does nothing for other shapes
		 * @param coordinate The object coordinate of the point
		*/
		virtual void GetCoordinate(Geometry::Point3D& coordinate) = 0;
		/**
		* @brief Gets the area of the shape. Is negative if the shape lacks an area
		* @return The area of the shape
		*/
		virtual double GetArea() = 0;
		/**
		 * @brief Gets the length of the shape: combined segment length for polyline and perimeter (circumference) for polygon. Is negative if the shape lacks a length
		 * @return The length of the shape
		*/
		virtual double GetLength() = 0;
		/**
		* @brief Gets the volume of the shape. Is negative if the shape lacks a volume
		* @return The volume of the shape
		*/
		virtual double GetVolume() = 0;
		/**
		* @brief Returns and possibly calculates the heightprofile of the shape if the shape is a LineShape
		* @return Returns the heightprofile if the shape is a LineShape, otherwise NULL
		*/
		virtual Geometry::HeightProfilePtr GetHeightProfile() = 0;
		/**
		* @brief Says if the mouseclick would select the current shape
		* @param mouseClick The user input indicating what shape to select
		* @return True if the shape should be selected, false otherwise
		*/
		virtual bool Select(Geometry::Point mouseClick) = 0;
		/**
		* @brief Used for selecting a point within a shape. If the mouseclick is not close to a point, a point is created on that location.
		* @param mouseClick The user input indicating what point to select
		* @return True if a point could be selected, false otherwise
		*/
		virtual bool GetPoint(Geometry::Point mouseClick) = 0;
		/**
		* @brief Gives access to the rendering coordinates of the shape so that it can be rendered
		* @param index The index of the rendering coordinate to return
		* @return The specified rendering coordinate
		*/
		virtual Geometry::Point GetRenderingPoint(int index) = 0;
		/**
		* @brief Allows the adding of points to shapes aside from PointShape. If the index is too big the operation will fail.
		* @param newPoint The point to add
		* @param index The place of the shape the point should be added to
		* @return True if the point was added, false if the point was not added
		*/
		virtual bool AddPoint(Geometry::Point newPoint, int index) = 0;
		/**
		* @brief When a point of a shape has been moved this method should be called.
		*
		* It should first update the 3D-coordinate of the modified shape, and then recalculate all measurements.
		* @param g A geometry object that allows for making coordinate changes
		*/
		virtual void UpdateCalculations(Geometry& g) = 0;
		/**
		* @brief Gives access to the number of points in a shape
		* @return The number of points in the shape
		*/
		virtual int GetNumberOfPoints() = 0;
		/**
		* @brief Says if the shape is completed.
		*
		* A point is completed if the point is set, a line is completed if there are 2 or more points, and a polygon is completed if there are 3 or more points.
		*
		* @return Whether or not the shape is completed.
		*/
		virtual bool IsCompleted() = 0;

		/**
		 * @brief Retrieves the index that the current mouse position would occupy in the shape if pressed
		 * @param mousePoint The current mouse position
		 * @return The index
		*/
		virtual int GetPossibleIndex(Geometry::Point mousePoint) = 0;

		/**
		 * @brief Deselects any point that might be selected
		*/
		virtual void DeselectPoint() = 0;

		/**
		 * @brief Moves the selected point to the specified position
		 * @param mousePoint The position to move the point to
		*/
		virtual void MoveSelectedPoint(Geometry::Point mousePoint) = 0;
		/**
		* @brief Paint the shape on screen.
		*
		* Uses "old style" direct commands and is thus intended only for relatively few objects.
		* The alternative is to create OpenGL enabled GpuBuffer:s for vertexes and colors and use ImageGLBase::SetVertexBuffers.
		*
		*
		* @param selected Defines if the shape should be drawn as if it is selected or not. Default is false
		* @param mousepoint The point the mouse currently occupies. Should only be set when the shape is selected. Default is (0,0)
		* Uses the rendering coordinates
		*/
		virtual void Draw(bool selected = false, bool isMeasuring = false, Geometry::Point mousePoint = Geometry::Point(0, 0)) = 0;

		/**
			* @brief Method that returns the type of the shape
			* @return The type of the shape
		*/
		ShapeType GetType();

		/**
			* @brief Method that returns the color of the shape
			* @return The color of the shape
		*/
		wxColour GetColor();

		/**
		* @brief Destructor, removes the wxPanel when the shape is deleted
		*/
		virtual ~Shape();

	protected:
		/**
		* @brief Constructor
		* @param t The type of the shape
		* @param c The color of the shape
		*/
		Shape(ShapeType t, wxColour c);
		int cNextInsertIndex;
		int cSelectedPointIndex;
		ShapeType cType;
		wxColour cColor;
	};
	typedef boost::shared_ptr<Shape> ShapePtr; //!< Smart pointer to Shape

	class PointShape : public Shape {
	public:

		PointShape(wxColour c);
		PointShape(wxColour c, wxString& wkt);
		~PointShape();

		void GetCoordinate(Geometry::Point3D& coordinate) override;
		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		Geometry::HeightProfilePtr GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		bool GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		int GetNumberOfPoints() override;
		bool IsCompleted() override;
		void Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) override;
		void DeselectPoint() override;
		void MoveSelectedPoint(Geometry::Point mousePoint) override;
		int GetPossibleIndex(Geometry::Point mousePoint) override;
		bool GetWKT(std::string& wkt) override;
		Geometry::Point3D GetCoordinates();

	private:
		Geometry::Point3D cCoordinate;
		Geometry::Point cRenderCoordinate;
		bool cIsComplete;
	};

	class LineShape : public Shape {
	public:
		LineShape(wxColour c);
		LineShape(wxColour c, wxString& wkt);
		~LineShape();
		void GetCoordinate(Geometry::Point3D& coordinate) override;
		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		Geometry::HeightProfilePtr GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		bool GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		bool IsCompleted() override;
		void DeselectPoint() override;
		void MoveSelectedPoint(Geometry::Point mousePoint) override;
		int GetNumberOfPoints() override;
		void Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) override;
		int GetPossibleIndex(Geometry::Point mousePoint) override;
		bool GetWKT(std::string& wkt) override;
		boost::geometry::model::linestring<Geometry::Point3D> GetCoordinates();
	private:
		double cLength;
		Geometry::VectorTrain3DPtr cCoordinates;
		Geometry::VectorTrainPtr cRenderCoordinates;
		Geometry::HeightProfilePtr cProfile;
	};

	class PolygonShape : public Shape {
	public:
		PolygonShape(wxColour c);
		PolygonShape(wxColour c, wxString& wkt);
		PolygonShape(Geometry::PolygonPtr pPolygon, wxColour c = wxColour(255,0,0, 64));
		virtual ~PolygonShape();
		void GetCoordinate(Geometry::Point3D& coordinate) override;
		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		Geometry::HeightProfilePtr GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		bool GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		bool IsCompleted() override;
		void DeselectPoint() override;
		void MoveSelectedPoint(Geometry::Point mousePoint) override;
		int GetNumberOfPoints() override;
		bool GetWKT(std::string& wkt) override;
		Geometry::Polygon3D GetCoordinates();

		/**
		* @brief Define what to draw
		* @param bPolygon Draw filled polygon (with transparency if set)
		* @param bLines Draw tesselated lines
		* @param bPoints Draw vertexes
		* @todo Tesselated lines are fine for illustration but should be changed to only original boundary lines
		* @todo This could be implemented with flags/enums in a new Shape::SetDrawMode for all shape types to enable a simple \c Shape(Line|Point|Polygon)::Draw() when it is time to draw each shape
		*/
		void SetDrawMode(bool bPolygon = true, bool bLines = false, bool bPoints = false);
		void Draw(bool selected, bool isMeasuring, Geometry::Point mousePoint) override;
		int GetPossibleIndex(Geometry::Point mousePoint) override;
	private:
		/**
		 * @brief Tesselate polygon.
		 *
		 * Handles concave polygons and also interior holes in polygons
		*/
		void Tesselate();
		double cLength;
		double cArea;
		double cVolume;
		Geometry::Polygon3DPtr cCoordinates;
		Geometry::PolygonPtr cRenderCoordinates;
		TESStesselator* cpTesselator;
		bool cbDrawPolygon, cbDrawLines, cbDrawPoints;
	};

}