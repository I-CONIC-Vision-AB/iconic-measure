#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicSensor/Camera.h>
#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <wx/colour.h>

namespace iconic {
	/**
	 * @brief Handles geometries such as polygons and transformations from 2D image to 3D object space.
	 *
	 * Not implemented on GPU and thus primarily for relatively few objects, e.g. interactive measurements.
	*/
	class ICONIC_MEASURE_COMMON_EXPORT Geometry {
	public:
		typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point; //!< 2D double precision point
		typedef boost::geometry::model::polygon<Point, false, true> Polygon; //!< ccw, closed polygon
		typedef boost::shared_ptr<Polygon> PolygonPtr; //!< Smart pointer to a 2D polygon

		typedef boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> Point3D; //!< 3D double precision point
		typedef boost::geometry::model::polygon<Point3D, false, true> Polygon3D; //!< ccw, closed polygon
		typedef boost::shared_ptr<Polygon3D> Polygon3DPtr; //!< Smart pointer to a 3D polygon

		typedef boost::shared_ptr<Point> PointPtr; //!< Smart pointer to a 2D point

		typedef boost::geometry::model::linestring<Point> VectorTrain;
		typedef boost::shared_ptr<VectorTrain> VectorTrainPtr;

		typedef boost::geometry::model::linestring<Point3D> VectorTrain3D;
		typedef boost::shared_ptr<VectorTrain3D> VectorTrain3DPtr;

		enum Colours {
			RED=0, GREEN=1, CYAN=2, MAGENTA=3, YELLOW=4, CERISE=5
		};

		/**
		* @brief Secondary type to Shape that defines how the shape should be treated calculation and rendering wise
		*/
		enum ShapeType {
			PolygonType, //!< A two-dimensional shape made up of multiple points
			LineType, //!< A one-dimensional shape made up of multiple points
			PointType //!< A single point
		};

		struct HeightProfile {};

		class Shape {
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
			virtual PointPtr GetPoint(Geometry::Point mouseClick) = 0;
			/**
				* @brief Gives access to the rendering coordinates of the shape so that it can be rendered
				* @param index The index of the rendering coordinate to return
				* @return The specified rendering coordinate
			*/
			virtual Point GetRenderingPoint(int index) = 0;
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
				* @brief Method that returns the type of the shape
				* @return The type of the shape
			*/
			ShapeType GetType();

			/**
				* @brief Method that returns the color of the shape
				* @return The color of the shape
			*/
			wxColour GetColor();

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
			PointPtr GetPoint(Geometry::Point mouseClick) override;
			Point GetRenderingPoint(int index) override;
			bool AddPoint(Geometry::Point newPoint, int index) override;
			void UpdateCalculations(Geometry& g) override;
			int GetNumberOfPoints() override;
			bool IsCompleted() override;

		private:
			Point3D coordinate;
			Point renderCoordinate;
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
			PointPtr GetPoint(Geometry::Point mouseClick) override;
			Point GetRenderingPoint(int index) override;
			bool AddPoint(Geometry::Point newPoint, int index) override;
			void UpdateCalculations(Geometry& g) override;
			bool IsCompleted() override;
			int GetNumberOfPoints() override;

		private:
			double length;
			VectorTrain3DPtr coordinates;
			VectorTrainPtr renderCoordinates;
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
			PointPtr GetPoint(Geometry::Point mouseClick) override;
			Point GetRenderingPoint(int index) override;
			bool AddPoint(Geometry::Point newPoint, int index) override;
			void UpdateCalculations(Geometry& g) override;
			bool IsCompleted() override;
			int GetNumberOfPoints() override;

		private:
			double length;
			double area;
			double volume;
			Polygon3DPtr coordinates;
			PolygonPtr renderCoordinates;
		};

			

		/**
		 * @brief Constructor
		*/
		Geometry();

		/**
		 * @brief Helper to create a smart pointer to an image polygon
		 * @param n Number of points in polygon
		 * @return Smart pointer to a polygon
		*/
		static PolygonPtr CreatePolygon(size_t n = 0);

		/**
		 * @brief Helper to create a smart pointer to an object polygon
		 * @param n Number of points in polygon
		 * @return Smart pointer to a polygon
		*/
		static Polygon3DPtr CreatePolygon3D(size_t n = 0);

		/**
		 * @brief Allocate 3D polygon so that it matches 2D polygon.
		 *
		 * Allocate the same number of points in outer boundary, the same number of interior polygons and the same number of points in the interior polygons.
		 * @param poly2d Input 2D polygon
		 * @param poly3d Output 3D polygon
		 * @return True on success, false if poly2d or poly3d is null
		 * @sa ImageToObject
		*/
		static bool Initialize3DPolygon(const PolygonPtr poly2d, const Polygon3DPtr poly3d);

		/**
		 * @brief Transform image coordinates to object coordinates.
		 *
		 * Calls Initialize3DPolygon first
		 * @param pImage Image polygon
		 * @param pObject Object polygon
		 * @return True n success
		 * @sa Initialize3DPolygon
		*/
		bool ImageToObject(iconic::Geometry::PolygonPtr pImage, iconic::Geometry::Polygon3DPtr pObject);

		/**
		 * @brief Transform image coordinate to object coordinate
		 * @param pImage Image point
		 * @param pObject Object point
		 * @return True on success
		*/
		bool ImageToObject(const iconic::Geometry::Point& pImage, iconic::Geometry::Point3D& pObject) const;

		/**
		 * @brief Transforms a 2D vector to 3D.
		 * @param vIn Input 2D image points
		 * @param vOut Output 3D object points. Must be allocated to the same size as vIn before call.
		 * @return True on success, false if image to object transformation failed.
		*/
		bool ImageToObject(const std::vector<iconic::Geometry::Point>& vIn, std::vector<iconic::Geometry::Point3D>& vOut);

		/**
		 * @brief Transform from image/camera system to pixel coordinate system
		 * @param imagePt Image/camera point
		 * @param pixelPt Pixel point
		*/
		void ImageToPixel(const Geometry::Point& imagePt, Geometry::Point& pixelPt) const;
		void ImageToPixel(const Eigen::Vector2d& imagePt, Geometry::Point& pixelPt) const;

		/**
		 * @brief Get height from depth map
		 * @param x Pixel x/column coordinate
		 * @param y Pixel y/row coordinate
		*/
		bool GetZ(const int x, const int y, double& Z) const;

		/**
		 * @brief Contains a defined list of colours that have been tested to look good in the program which can be used for the shapes
		 * @param c The requested colour
		 * @return The colour in a wxColour object
		*/
		wxColour GetColour(Colours c) const;

		std::vector<float> cDepthMap;				//!< Depth map with Z values. Size is cImageSize[0]*cImageSize[1] 
		iconic::CameraPtr cpCamera;					//!< Camera transforming 3D object points to 2D image/camera coordinates to 
		iconic::Camera::ECameraType cCameraType;	//!< Camera classification to enable faster transformations when possible
		size_t cImageSize[2];						//!< Image size (width,height)
		Eigen::Matrix3d cCameraToPixelTransform;	//!< Matrix transforming from image/camera system to pixel system in homogeneous coordinates
	};
}