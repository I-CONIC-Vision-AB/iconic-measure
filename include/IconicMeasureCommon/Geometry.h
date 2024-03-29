#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicSensor/Camera.h>
#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <wx/wx.h>
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

		typedef boost::geometry::model::linestring<Point> VectorTrain; //!< 2D vector train
		typedef boost::shared_ptr<VectorTrain> VectorTrainPtr; //!< Smart pointer to a 2D vector train

		typedef boost::geometry::model::linestring<Point3D> VectorTrain3D; //!< 3D vector train
		typedef boost::shared_ptr<VectorTrain3D> VectorTrain3DPtr; //!< Smart pointer to a 3D vector train

		/**
		 * @brief Unimplemented type for storing data regarding heightprofile of line
		*/
		struct HeightProfile {};
		typedef boost::shared_ptr<HeightProfile> HeightProfilePtr; //!< Smart pointer to heightprofile data

		/**
		 * @brief Predefined colour values for the shapes
		*/
		enum Colours {
			RED = 0, GREEN = 1, CYAN = 2, MAGENTA = 3, YELLOW = 4, CERISE = 5
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
		/**
		 * @brief Transform from image/camera system to pixel coordinate system
		 * @param imagePt Image/camera point
		 * @param pixelPt Pixel point
		*/
		void ImageToPixel(const Eigen::Vector2d& imagePt, Geometry::Point& pixelPt) const;

		/**
		 * @brief Get height from depth map
		 * @param x Pixel x/column coordinate
		 * @param y Pixel y/row coordinate
		 * @param Z The z coordinate 
		 * @return True if the z-coordinate could be gotten, false otherwise
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