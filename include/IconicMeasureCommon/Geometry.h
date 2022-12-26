#pragma once
#include <IconicMeasureCommon/exports.h>

#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

namespace iconic {
	/**
	 * @brief Handles geometries such as polygons
	*/
	class ICONIC_MEASURE_COMMON_EXPORT Geometry {
	public:
		typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point; //!< 2D double precision point
		typedef boost::geometry::model::polygon<Point, false, true> Polygon; //!< ccw, closed polygon
		typedef boost::shared_ptr<Polygon> PolygonPtr; //!< Smart pointer to a 2D polygon

		typedef boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> Point3D; //!< 3D double precision point
		typedef boost::geometry::model::polygon<Point3D, false, true> Polygon3D; //!< ccw, closed polygon
		typedef boost::shared_ptr<Polygon3D> Polygon3DPtr; //!< Smart pointer to a 3D polygon

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
	};
}