#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicGpu/MetaDataHandler.h>
#include <IconicSensor/Camera.h>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/SidePanel.h>
#include <IconicMeasureCommon/Shape.h>
#include <IconicMeasureCommon/MeasureEvent.h>
#include <IconicMeasureCommon/DrawEvent.h>
#include <wx/wx.h>

namespace iconic {

	/**
	 * @brief Sets up depth map and camera for current frame.
	 *
	 * MeasureHandler::OnNextFrame is called for every frame and looks for camera and depth map.
	 *
	 * A container can hold polygons (see MeasureHandler::AddImagePolygon), Currently they are only valid for the current frame, but later they may contain object polygons for a complete area.
	 */
	class ICONIC_MEASURE_COMMON_EXPORT MeasureHandler : public iconic::gpu::MetaDataHandler {
	public:

		/**
		 * @brief Constructor
		*/
		MeasureHandler();
		/**
		 * @brief Destructor
		*/
		~MeasureHandler();
		/**
		 * @brief Called for each frame/image
		 * @param pProperties Image properties of the frame
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
		 * @brief Read depth map and camera file.
		 * @sa ReadDepthMap ReadCamera
		*/
		virtual bool Parse();

		/**
		 * @brief Append the polygon to aggregated polygons
		 * @param pPolygon Image polygon
		 * @sa ImageToObject
		*/
		void AddImagePolygon(iconic::Geometry::PolygonPtr pPolygon);

		/**
		 * @brief Get smart pointer to a camera
		 * @return smart pointer to a camera
		 * @sa ReadCamera
		*/
		iconic::CameraPtr GetCamera();

		/**
		 * @brief Returns depth map.
		 *
		 * The vector has \c width*height floating point Z values,
		 * where \c width*height is the size of the original image.
		 * @return depth map
		 * @sa ReadDepthMap GetImageSize
		*/
		std::vector<float>& GetDepthMap();

		/**
		 * @brief Returns image size.
		 * @param width Width
		 * @param height Height
		*/
		void GetImageSize(size_t& width, size_t& height);

		/**
		 * @brief Transform image coordinates to object coordinates.
		 *
		 * Calls Geometry::Initialize3DPolygon first
		 * @param pImage Image polygon
		 * @param pObject Object polygon
		 * @return True n success
		 * @sa Geometry::Initialize3DPolygon Geometry::ImageToObject
		*/
		bool ImageToObject(iconic::Geometry::PolygonPtr pImage, iconic::Geometry::Polygon3DPtr pObject);

		/**
		 * @brief Transform image coordinate to object coordinate
		 * @param pImage Image point
		 * @param pObject Object point
		 * @return True on success
		 * @sa Geometry::ImageToObject
		*/
		bool ImageToObject(const iconic::Geometry::Point& pImage, iconic::Geometry::Point3D& pObject);

		/**
		 * @brief Transforms a 2D vector to 3D.
		 * @param vIn Input 2D image points
		 * @param vOut Output 3D object points. Must be allocated to the same size as vIn before call.
		 * @return True on success, false if image to object transformation failed.
		 * @sa Geometry::ImageToObject
		*/
		bool ImageToObject(const std::vector<iconic::Geometry::Point>& vIn, std::vector<iconic::Geometry::Point3D>& vOut);

		/**
		 * @brief Create new shape and set it to selected
		 * @param type The type of shape to instantiate
		 * @return True if new shape was instantiated, false if selectedShape was already set
		*/
		bool InstantiateNewShape(iconic::ShapeType type);

		/**
		 * @brief Allows for VideoPlayerFrame to request changes to the selected shape
		 * @param imgP The coordinates that the change relates to
		 * @param modification The type of change
		 * @param e The event that will be raised if a shape has been updated
		 * @return True if the event should be raised, false otherwise
		*/
		bool ModifySelectedShape(Geometry::Point imgP, MeasureEvent::EAction modification, DataUpdateEvent& e);

		/**
		 * @brief Handles finished measurement so that new measurements are added to shapes and altered shapes are altered
		 * @param instantiate_new True means to instantiate another shape after deselecting the current shape, only false when changing shape-type in toolbar
		*/
		void HandleFinishedMeasurement(bool instantiate_new = true);

		/**
		 * @brief If the selected shape has less points than the minimum necessary for the shapetype, delete it.
		*/
		void DeleteSelectedShapeIfIncomplete();

		/**
		 * @brief selectedShape is set to a new shape based on input coordinates
		 * @param p the point of which the to be selected polygon is placed
		 * @return The type of shape that was selected
		*/
		iconic::ShapeType SelectShapeFromCoordinates(Geometry::Point p);

		/**
		 * @brief Deletes the shape specified by selectedShapeIndex
		 * @return The index of the deleted shape, -1 if nothing was deleted
		*/
		int DeleteSelectedShape();

		/**
		 * @brief Method to clear all shapes, called before program exit
		*/
		void ClearShapes();

		/**
		 * @brief Responds to DrawEvents from ImageCanvas
		 * @param e The draw event data
		*/
		void OnDrawShapes(DrawEvent& e);



		/**
		 * @brief Get the WKT representations of all the shapes
		 * @param wkt The string containing the WKT strings
		 * @return True if atleast one shape was converted to WKT, false otherwise
		*/
		bool GetWKT(std::string& wkt);

		/**
		 * @brief Creates a shape from a WKT string
		 * @param wkt The WKT representation of a shape
		 * @param e The event to raise if a shape was loaded
		 * @return True if a shape was loaded, false otherwise	
		*/
		bool LoadWKT(wxString& wkt, DataUpdateEvent& e);

		/**
		 * @brief Deletes all stored shapes
		*/
		void DeleteAllShapes();



	private:

		/**
		 * @brief Read depth map from file.
		 *
		 * Called by Parse
		*/
		bool ReadDepthMap();

		/**
		 * @brief Read camera from file.
		 *
		 * Called by Parse. Calls CheckCamera
		*/
		bool ReadCamera();

		/**
		 * @brief Determine camera type.
		 *
		 * Used to make image to object transformation as fast as possible.
		 * Called by Parse
		*/
		void CheckCamera();

		SidePanel* sidePanel;
		wxString cImageFileName;
		wxString cDepthMapFileName;
		wxString cCameraFileName;
		iconic::gpu::ImagePropertyPtr cpProperties;
		bool cbIsParsed;
		std::vector<iconic::Geometry::PolygonPtr> cvImagePolygon; // Vector of polygons in camera coordinates (not screen coordinates)
		std::vector<iconic::Geometry::Polygon3DPtr> cvObjectPolygon; // Vector of polygons with 3D object coordinates (XYZ)
		std::vector <ShapePtr> cvShapes;
		ShapePtr cpSelectedShape;
		int cSelectedShapeIndex;
		Geometry cGeometry;
	};

	typedef boost::shared_ptr<MeasureHandler> MeasureHandlerPtr; //!< Smart pointer to MeasureHandler
}
