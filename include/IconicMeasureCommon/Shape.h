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


namespace iconic {

	/**
	* @brief Secondary type to Shape that defines how the shape should be treated calculation and rendering wise
	*/
	enum ShapeType {
		PolygonType, //!< A two-dimensional shape made up of multiple points
		LineType, //!< A one-dimensional shape made up of multiple points
		PointType //!< A single point
	};

	struct HeightProfile {};

	/**
	 * @brief The abstract base class for the Shapes
	 * 
	 * Defines methods that a shape must implement to be considered a viable shape object.
	*/
	class ICONIC_MEASURE_COMMON_EXPORT Shape {
	public:
		/**
			* @brief Gets the area of the shape. Is negative if the shape lacks an area
			* @return The area of the shape
		*/
		virtual double GetArea() = 0;
		/**
		 * @brief Gets the length of the shape: combined segment length for polyline and perimiter (circumference) for polygon. Is negative if the shape lacks a length
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
		virtual boost::shared_ptr<HeightProfile> GetHeightProfile() = 0;
		/**
			* @brief Says if the mouseclick would select the current shape
			* @param mouseClick The user input indicating what shape to select
			* @return True if the shape should be selected, false otherwise
		*/
		virtual bool Select(Geometry::Point mouseClick) = 0;
		/**
			* @brief Used for selecting a point within a shape
			* @param mouseClick The user input indicating what point to select
			* @return A pointer to the selecter point, or NULL if no point has been selected
		*/
		virtual Geometry::PointPtr GetPoint(Geometry::Point mouseClick) = 0;
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
		 * @brief Tells if the shape has an associated wxPanel to it
		 * @return True if its panel has been created, false otherwise
		*/
		bool HasPanel();

		/**
		 * @brief Sets the associated wxPanel of the shape
		*/
		void SetPanel(wxPanel* panel);

		/**
		 * @brief Returns the associated wxPanel of the shape
		 * @return The associated wxPanel of the shape
		*/
		wxPanel* GetPanel();
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
		~Shape();


	protected:
		/**
			* @brief Constructor
			* @param t The type of the shape
			* @param c The color of the shape
		*/
		Shape(ShapeType t, wxColour c);



		int selectedPointIndex;
		ShapeType type;
		wxColour color;
		wxPanel* panel;
	};

	class PointShape : Shape {
	public:

		PointShape(wxColour c);
		~PointShape();

		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		boost::shared_ptr<HeightProfile> GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		Geometry::PointPtr GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		int GetNumberOfPoints() override;
		bool IsCompleted() override;

	private:
		Geometry::Point3D coordinate;
		Geometry::Point renderCoordinate;
		bool isComplete;
	};

	class LineShape : Shape {
	public:
		LineShape(wxColour c);
		~LineShape();
		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		boost::shared_ptr<HeightProfile> GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		Geometry::PointPtr GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		bool IsCompleted() override;
		int GetNumberOfPoints() override;
	private:
		double length;
		Geometry::VectorTrain3DPtr coordinates;
		Geometry::VectorTrainPtr renderCoordinates;
		boost::shared_ptr<HeightProfile> profile;
	};

	class PolygonShape : Shape {
	public:
		PolygonShape(wxColour c);
		~PolygonShape();
		double GetArea() override;
		double GetLength() override;
		double GetVolume() override;
		boost::shared_ptr<HeightProfile> GetHeightProfile() override;
		bool Select(Geometry::Point mouseClick) override;
		Geometry::PointPtr GetPoint(Geometry::Point mouseClick) override;
		Geometry::Point GetRenderingPoint(int index) override;
		bool AddPoint(Geometry::Point newPoint, int index) override;
		void UpdateCalculations(Geometry& g) override;
		bool IsCompleted() override;
		int GetNumberOfPoints() override;
	private:
		double length;
		double area;
		double volume;
		Geometry::Polygon3DPtr coordinates;
		Geometry::PolygonPtr renderCoordinates;
	};

}