#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicSensor/Camera.h>
#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

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

		enum ShapeType { PolygonShape, VectorTrainShape };

		struct Color {
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			unsigned char alpha;
		};

		struct Shape {
			Color color;
			double length;
			double area;
			bool completed;


			ShapeType type;
			Polygon3DPtr dataPointer;

			Shape(ShapeType t, Polygon3DPtr ptr) {
				color = Color { 255, 0, 0, 255 };
				length = -1;
				area = -1;
				completed = false;

				type = t;
				dataPointer = ptr;
			}
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
			
		std::vector<float> cDepthMap;				//!< Depth map with Z values. Size is cImageSize[0]*cImageSize[1] 
		iconic::CameraPtr cpCamera;					//!< Camera transforming 3D object points to 2D image/camera coordinates to 
		iconic::Camera::ECameraType cCameraType;	//!< Camera classification to enable faster transformations when possible
		size_t cImageSize[2];						//!< Image size (width,height)
		Eigen::Matrix3d cCameraToPixelTransform;	//!< Matrix transforming from image/camera system to pixel system in homogeneous coordinates
	};
}