#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicSensor/Camera.h>
#include <boost/shared_ptr.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <wx/wx.h>

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
		/**
		 * @brief Secondary type to Shape that defines how the shape should be treated calculation and rendering wise
		*/
		enum ShapeType {
			PolygonType, //!< A two-dimensional shape made up of multiple points
			LineType, //!< A one-dimensional shape made up of multiple points
			PointType //!< A single point
		};

		/**
		 * @brief Secondary type to Shape that defines rendering color.
		 * 
		 * Contains the values red, green, blue, alpha
		*/
		struct Color {
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
		};
		struct HeightProfile{};

		class Shape {
		public:
			Shape(ShapeType t, Color c) {
				type = t;
				color = c;
				selectedPointIndex = -1;
				panel = nullptr;
			};

			~Shape() {
				if (panel) {
					panel->Destroy();
				}
			}
			/**
			 * @brief Gets the area of the shape. Is negative if the shape lacks an area
			 * @return The area of the shape
			*/
			virtual double GetArea() = 0;
			/**
			 * @brief Gets the length of the shape. Is negative if the shape lacks a length
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
			 * @brief Method that returns the type of the shape
			 * @return The type of the shape
			*/
			ShapeType GetType() { return this->type; }

			/**
			 * @brief Method that returns the color of the shape
			 * @return The color of the shape
			*/
			Color GetColor() { return this->color; }


			/**
			 * @brief Method that creates panel of shape
			*/
			virtual void CreatePanel(wxWindow* side_panel) = 0;


		protected:
			int selectedPointIndex;
			ShapeType type;
			Color color;
			wxPanel* panel;
		};

		class PointShape : Shape {
		public:

			PointShape(Color c) : Shape(ShapeType::PointType, c) {
				renderCoordinate = Point(-1,-1);
				coordinate = Point3D(-1, -1, -1);
			}
			~PointShape() {}

			void CreatePanel(wxWindow* side_panel) override;

			double GetArea() override {
				return -1;
			}
			double GetLength() override {
				return -1;
			}
			double GetVolume() override {
				return -1;
			}
			boost::shared_ptr<HeightProfile> GetHeightProfile() override {
				return NULL;
			}
			bool Select(Geometry::Point mouseClick) override {
				// Add ImageCanvas::GetScale() as argument?
				if (boost::geometry::distance(mouseClick, renderCoordinate) < 0.005f) { // Should depend on the zoom amount
					return true;
				}
				else {
					return false;
				}
			}
			PointPtr GetPoint(Geometry::Point mouseClick) override {
				// This should occur by selecting the entire shape instead
				return NULL;
			}
			Point GetRenderingPoint(int index) override {
				return renderCoordinate;
			}
			bool AddPoint(Geometry::Point newPoint, int index) override {
				// Adding a point only occurs when defining the point
				if (boost::geometry::equals(renderCoordinate, Point(-1, -1))) {
					renderCoordinate = newPoint;
					// UpdateCalculations should be called after the point has been defined
					return true;
				}
				return false;
			}

			void UpdateCalculations(Geometry& g) override {
				if (!g.ImageToObject(this->renderCoordinate, this->coordinate))
				{
					wxLogError(_("Could not compute image-to-object coordinates for measured point"));
					return;
				}
			}
			int GetNumberOfPoints() override {
				return 1;
			}

		private:
			Point3D coordinate;
			Point renderCoordinate;
		};

		class LineShape : Shape{
		public:
			LineShape(Color c) : Shape(ShapeType::LineType, c) {
				renderCoordinates = VectorTrainPtr(new VectorTrain);
				coordinates = VectorTrain3DPtr(new VectorTrain3D);
			}
			~LineShape() {}

			void CreatePanel(wxWindow* side_panel) override;

			double GetArea() override {
				return -1;
			}
			double GetLength() override {
				return length;
			}
			double GetVolume() override {
				return -1;
			}
			boost::shared_ptr<HeightProfile> GetHeightProfile() override {
				return profile;
			}
			bool Select(Geometry::Point mouseClick) override {
				if (boost::geometry::distance(mouseClick, *renderCoordinates.get()) < 0.001f) { // Should depend on the zoom amount
					return true;
				}
				else {
					return false;
				}
			}
			PointPtr GetPoint(Geometry::Point mouseClick) override {
				int i = 0;
				for (Point& p : *renderCoordinates) {
					if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
						selectedPointIndex = i;
						return PointPtr(&p);
					}
					i++;
				}
				//https://gis.stackexchange.com/questions/127783/distance-between-a-point-and-linestring-postgis-geos-vs-boost
				return NULL;
			}

			Point GetRenderingPoint(int index) override {
				if (index == -1)
					return renderCoordinates->back();
				else
					return renderCoordinates->at(index);
			}
			bool AddPoint(Geometry::Point newPoint, int index) override {
				if (index == -1)
					renderCoordinates->push_back(newPoint);
				else
					renderCoordinates->insert(renderCoordinates->begin() + index, newPoint);

				return true;
			}

			void UpdateCalculations(Geometry& g) override {
				if (this->renderCoordinates->size() <= 1) return;
				this->coordinates->clear();
				Point3D p;
				for (int i = 0; i < renderCoordinates->size(); i++) {
					if (!g.ImageToObject(this->renderCoordinates->at(i), p))
					{
						wxLogError(_("Could not compute image-to-object coordinates for measured point"));
						return;
					}
					this->coordinates->push_back(p);
				}

				length = boost::geometry::length(*renderCoordinates);

				Point3D start, end;
				Point3D differenceVec;
				std::vector<double> zValues;
				double norm;

				for (int i = 0; i < coordinates->size() - 1; i++) {
					start = coordinates->at(i);
					end = coordinates->at(i + 1);

					differenceVec.set<0>(end.get<0>() - start.get<0>());
					differenceVec.set<1>(end.get<1>() - start.get<1>());
					differenceVec.set<2>(end.get<2>() - start.get<2>());

					norm = boost::geometry::distance(start, end);
					
					wxLogVerbose(_("Norm " + std::to_string(i) + " : " + std::to_string(norm)));

					/*
					The code below is not implemented correctly since the scale of the norm is based on the object coordinates
					The sampling will at most take two points which is not great
					This should be fixed after discussions with I-CONIC
					-
					Alex
					*/

					//differenceVec.set<0>(differenceVec.get<0>() / norm);
					//differenceVec.set<1>(differenceVec.get<1>() / norm);
					if (norm != 0)
						differenceVec.set<2>(differenceVec.get<2>() / norm);
					else
						norm = 0.0001;
					for (int j = 0; j < norm; j++) {
						zValues.push_back(start.get<2>() + differenceVec.get<2>() * j);
					}

				}
				// Calculate profile
			}

			int GetNumberOfPoints() override {
				return renderCoordinates->size();
			}

		private:
			double length;
			VectorTrain3DPtr coordinates;
			VectorTrainPtr renderCoordinates;
			boost::shared_ptr<HeightProfile> profile;
		};

		class PolygonShape : Shape {
		public:
			PolygonShape(Color c) : Shape(ShapeType::PolygonType, c) {
				renderCoordinates = PolygonPtr(new Polygon);
				coordinates = Polygon3DPtr(new Polygon3D);
			}
			~PolygonShape(){}

			void CreatePanel(wxWindow* side_panel) override;

			double GetArea() override {
				return area;
			}
			double GetLength() override {
				return length;
			}
			double GetVolume() override {
				return volume;
			}
			boost::shared_ptr<HeightProfile> GetHeightProfile() override {
				return NULL;
			}
			bool Select(Geometry::Point mouseClick) {

				// Add the first point again to the end of the polygon as you can above the first and last point otherwise, does not seem to be treated as closed
				this->renderCoordinates->outer().push_back(this->renderCoordinates->outer()[0]);
				
				if (boost::geometry::within(mouseClick, *renderCoordinates.get())) {
					this->renderCoordinates->outer().pop_back();
					return true;
				}
				else {
					this->renderCoordinates->outer().pop_back();
					return false;
				}
			}
			PointPtr GetPoint(Geometry::Point mouseClick) override {
				int i = 0;
				for (Point& p : renderCoordinates.get()->outer()) {
					if (boost::geometry::distance(mouseClick, p) < 0.005f) { // Should depend on the zoom amount
						selectedPointIndex = i;
						return PointPtr(&p);
					}
					i++;
				}
				/*
				* If a created point has not been created, a new point could be created here
				*/
				return NULL;
			}
			Point GetRenderingPoint(int index) override {
				if (index == -1)
					return renderCoordinates->outer().back();
				else
					return renderCoordinates->outer().at(index);
			}
			bool AddPoint(Geometry::Point newPoint, int index) override {
				if(index == -1)
					renderCoordinates->outer().push_back(newPoint);
				else
					renderCoordinates->outer().insert(renderCoordinates->outer().begin() + index, newPoint);
				
				return true;
			}
			void UpdateCalculations(Geometry& g) override {
				for (int i = 0; i < coordinates->outer().size(); i++) {
					if (!g.ImageToObject(this->renderCoordinates->outer().at(i), this->coordinates->outer().at(i)))
					{
						wxLogError(_("Could not compute image-to-object coordinates for measured point"));
						return;
					}
				}

				length = boost::geometry::length(renderCoordinates->outer());
				area = boost::geometry::area(renderCoordinates->outer());
				volume = area * 5; // Not a correct solution
			}

			int GetNumberOfPoints() override {
				return renderCoordinates->outer().size();
			}

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

		std::vector<float> cDepthMap;				//!< Depth map with Z values. Size is cImageSize[0]*cImageSize[1] 
		iconic::CameraPtr cpCamera;					//!< Camera transforming 3D object points to 2D image/camera coordinates to 
		iconic::Camera::ECameraType cCameraType;	//!< Camera classification to enable faster transformations when possible
		size_t cImageSize[2];						//!< Image size (width,height)
		Eigen::Matrix3d cCameraToPixelTransform;	//!< Matrix transforming from image/camera system to pixel system in homogeneous coordinates
	};
}