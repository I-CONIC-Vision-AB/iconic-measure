#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicGpu/MetaDataHandler.h>
#include <IconicSensor/Camera.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

/**
 * @brief Sets up depth map and camera for current frame.
 * 
 * IconicMeasureHandler::OnNextFrame is called for every frame and looks for camera and depth map. 
 * 
 * A container can hold polygons (see IconicMeasureHandler::AddImagePolygon), Currently they are only valid for the current frame, but later they may contain object polygons for a complete area.
 * 
 * @todo IconicMeasureHandler::Parse should load the camera and depth map 
*/
class ICONIC_MEASURE_COMMON_EXPORT IconicMeasureHandler : public iconic::gpu::MetaDataHandler {
public:
	/**
	 * @brief Type of camera.
	 * 
	 * Used to simplify object coordinate estimation if possible
	*/
	enum class ECameraType {
		IDENTITY,				//!< Identity calibration matrix, zero translation, zero rotation
		FOCAL_LENGTH,			//!< Focal length in calibration matrix, zero translation, zero rotation
		CALIBRATION_MATRIX,		//!< Full calibration matrix, zero translation, zero rotation
		FULL					//!< Full calibration matrix, non-zero translation and/or rotation
	};

	typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point; //!< 2D double precision point
	typedef boost::geometry::model::polygon<Point, false, true> Polygon; //!< ccw, closed polygon
	typedef boost::shared_ptr<Polygon> PolygonPtr; //!< Smart pointer to a 2D polygon

	typedef boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian> Point3D; //!< 3D double precision point
	typedef boost::geometry::model::polygon<Point3D, false, true> Polygon3D; //!< ccw, closed polygon
	typedef boost::shared_ptr<Polygon3D> Polygon3DPtr; //!< Smart pointer to a 3D polygon

	/**
	 * @brief Constructor
	*/
	IconicMeasureHandler();

	/**
	 * @brief Called for each frame/image
	 * @param filename Meta data file name
	 * @param frameNumber Frame or image number
	 * @param time Time stamp
	 * @param imSize Image size
	 * @param bDoParse Call \c Parse (e.g. to read image transformation). If only saving data while processing, parsing may be unnecessary.
	 * @return True on success
	 * @sa Parse
	*/
	virtual bool OnNextFrame(iconic::gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse = false);

	/**
	 * @brief Returns depth map file name
	 * @return depth map file name
	*/
	virtual wxString GetMetaFileName() const;

	/**
	 * @brief Read depth map and camera file
	*/
	virtual bool Parse();

	/**
	 * @brief Append the polygon to aggregated polygons
	 * @param pPolygon Image polygon
	 * @param bAddObjectPolygon If true, then an object polygon is created, computed and added as well
	 * @sa ImageToObject
	*/
	void AddImagePolygon(PolygonPtr pPolygon, bool bAddObjectPolygon = true);

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

	iconic::CameraPtr GetCamera();

	std::vector<float>& GetDepthMap();

	void GetImageSize(size_t& width, size_t& height);
private:
	/**
	 * @brief Transform image coordinates to object coordinates
	 * @param pImage Image polygon
	 * @param pObject Object polygon
	 * @return True n success
	 * @todo Does not handle interior polygons ("holes")
	*/
	bool ImageToObject(PolygonPtr pImage, Polygon3DPtr pObject);

	/**
	 * @brief Transform image coordinate to object coordinate
	 * @param pImage Image point
	 * @param pObject Object point
	 * @return True on success
	 * @todo Not implemented. Just copies the image point
	*/
	bool ImageToObject(const Point& pImage, Point3D& pObject);

	bool ReadDepthMap();
	bool ReadCamera();
	void CheckCamera();

	wxString cImageFileName;
	wxString cDepthMapFileName;
	wxString cCameraFileName;
	iconic::gpu::ImagePropertyPtr cpProperties;
	bool cbIsParsed;
	std::vector<PolygonPtr> cvImagePolygon; // Vector of polygons in image coordinates (not screen coordinates)
	std::vector<Polygon3DPtr> cvObjectPolygon; // Vector of polygons with 3D object coordinates (XYZ)
	std::vector<float> cDepthMap;
	iconic::CameraPtr cpCamera;
	ECameraType cCameraType;
};

typedef boost::shared_ptr<IconicMeasureHandler> IconicMeasureHandlerPtr;