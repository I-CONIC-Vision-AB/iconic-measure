#pragma once

#include <IconicMeasureCommon/exports.h>
#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <boost/shared_ptr.hpp>
#include <IconicVideo/ImageGLBase.h>
#include <IconicGpu/IconicLog.h>
#include <IconicMeasureCommon/MeasureHandler.h>
#include <IconicMeasureCommon/Geometry.h>

namespace iconic {
	/**
	 * @brief Image display window.
	 *
	 * Efficient display of images using e.g. GPU decoded image directly without copy to CPU.
	 * Uses OpenGL to render images.
	*/
	class ICONIC_MEASURE_COMMON_EXPORT ImageCanvas : public wxGLCanvas, public ImageGLBase {
	public:
		/**
		 * @brief How to handle mouse events
		*/
		enum class EMouseMode {
			MOVE,	//!< Move, pan, zoom in image
			MEASURE	//!< Measure in image
		};
		/**
		 * @brief Constructor
		 * @param parent Parent window
		 * @param canvasAttrs OpenGL attributes to be applied
		 * @param nDispWidth Display width
		 * @param nDispHeight Display height
		 * @param nTexWidth Image width
		 * @param nTexHeight Image height
		 * @param bUsePbo Use Pixel Binary Object (PBO) or not
		 * @param mHandlerPtr Pointer that allows access to list of shapes for rendering
		*/
		ImageCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs, unsigned int nDispWidth,
			unsigned int nDispHeight, unsigned int nTexWidth, unsigned int nTexHeight, bool bUsePbo, iconic::MeasureHandlerPtr mHandlerPtr);

		/**
		 * @brief Destructor
		*/
		virtual ~ImageCanvas();

		/**
		 * @brief Paints the image
		 * @param event unused
		*/
		virtual void OnPaint(wxPaintEvent& event);

		/**
		 * @brief Draws the currently selected shape.
		 * 
		 * This is run in addition to the "normal" rendering to make sure extra points are placed for modifications and clarity.
		 * 
		 * Uses "old style" direct commands and is thus intended only for relatively few objects.
		 * The alternative is to create OpenGL enabled GpuBuffer:s for vertexes and colors and use ImageGLBase::SetVertexBuffers.
		 * 
		*/
		virtual void DrawSelectedGeometry(boost::shared_ptr<iconic::Geometry::Shape> selectedShape);

		/**
		 * @brief Draws the supplied polygon in the supplied color
		 * 
		 * Uses "old style" direct commands and is thus intended only for relatively few objects.
		 * The alternative is to create OpenGL enabled GpuBuffer:s for vertexes and colors and use ImageGLBase::SetVertexBuffers.
		 * 
		 * @param coordinates The image-coordinates of the polygon that is to be drawn
		 * @param color Color of polygon
		 * 
		 * @deprecated
		*/
		virtual void DrawMeasuredPolygon(Geometry::PolygonPtr coordinates, iconic::Geometry::Color color);

		/**
		 * @brief Draws the supplied vector-train (line) in the supplied color
		 *
		 * Uses "old style" direct commands and is thus intended only for relatively few objects.
		 * The alternative is to create OpenGL enabled GpuBuffer:s for vertexes and colors and use ImageGLBase::SetVertexBuffers.
		 *
		 * @param coordinates The image-coordinates of the vector-train that is to be drawn
		 * @param color Color of vector-train
		 *
		 * @deprecated
		*/
		[[deprecated]] 
		virtual void DrawMeasuredVectorTrain(Geometry::PolygonPtr coordinates, iconic::Geometry::Color color);

		/**
		 * @brief Draws the supplied geometry in the supplied color with the supplied OpenGL drawtype
		 * 
		 * Generalizes the previous methods for different drawtypes.
		 * 
		 * Uses "old style" direct commands and is thus intended only for relatively few objects.
		 * The alternative is to create OpenGL enabled GpuBuffer:s for vertexes and colors and use ImageGLBase::SetVertexBuffers.
		 * 
		 * @param coordinates The coordinates to be rendered.
		 * @param color The color to render in.
		 * @param glDrawType The type of OpenGL type to render.
		 * @param useAlpha Specifies if the geometries alpha value should be used. False by default.
		*/
		virtual void DrawGeometry(Geometry::PolygonPtr coordinates, iconic::Geometry::Color color, int glDrawType, bool useAlpha = false);

		/**
		 * @brief Called when window is resized
		 * @param event Size event
		*/
		virtual void OnSize(wxSizeEvent& event);

		/**
		 * @brief Refreshes window on idle
		 * @param event unused
		*/
		virtual void OnIdle(wxIdleEvent& event);

		/**
		 * @brief Refreshes the image.
		 *
		 * Overload from ImageGLBase. Only refreshes here, but can set up window and image sizes etc.
		 * @param field_num unused
		 * @param refresh Refresh
		*/
		virtual void render(int field_num, bool refresh);

		/**
		 * @brief Setup OpenGL.
		 *
		 * No harm calling multiple times, since it will return directly if already initialized.
		*/
		virtual void initGL();

		/**
		 * @brief Set the associated OpenGl context as current
		*/
		virtual void SetCurrent();

		/**
		 * @brief Refresh the image window.
		 *
		 * Overload from ImageGLBase.
		*/
		virtual void refresh();

		/**
		 * @brief Swap double buffers.
		 *
		 * Calls OpenGL swap buffers
		 * @return Always true
		*/
		virtual bool SwapBuffers();

		/**
		 * @brief Set image title
		 * @param title Title
		*/
		virtual void SetTitle(char title[]);

		/**
		 * @brief Setup parameters for OpenGL.
		 *
		 * Sets up OpenGL model and projection modes for the current window and image sizes.
		*/
		void ResetProjectionMode();

		/**
		 * @brief Fit image to window.
		 * @param set Fit image to window.
		*/
		void SetFitToWindow(bool set = true);

		/**
		 * @brief Fit image to window
		 * @return Fit image to window
		*/
		bool GetFitToWindow() const;

		/**
		 * @brief
		 * @param event
		*/
		void OnMouse(wxMouseEvent& event);

		/**
		 * @brief
		 * @param event
		*/
		void OnMouseWheel(wxMouseEvent& event);

		/**
		 * @brief Set mouse mode.
		 *
		 * Toggle between moving (and zooming) or measuring
		 * @param mode Mouse mode
		*/
		void SetMouseMode(EMouseMode mode);

		/**
		 * @brief Get mouse mode.
		 * @return Mouse mode
		 * @sa SetMouseMode
		*/
		EMouseMode GetMouseMode() const;

		static boost::shared_ptr<wxGLContext> gpContext; //!< Singleton OpenGL context

		/**
		 * @brief Transform screen coordinates to normalized camera coordinates.
		 * @param pt Screen coordinates
		 * @param x x camera coordinate [-0.5..+0.5]
		 * @param y y camera coordinate [-0.5..+0.5]
		*/
		void ScreenToCamera(const wxPoint& pt, float& x, float& y);
	protected:
		/**
		 * @brief Handle mouse move.
		 *
		 * Pan around and zoom
		 * @param event Mouse position etc
		*/
		virtual void MouseMove(wxMouseEvent& event);

		/**
		 * @brief Handle measurement
		 * @param event
		*/
		virtual void MouseMeasure(wxMouseEvent& event);

		/**
		 * @brief Fit image to window
		*/
		void FitToWindow();

	private:
		bool cbFitToWindow;
		wxPoint cLastMousePos;
		EMouseMode cMouseMode;
		wxSize cLastClientSize;
		GLdouble cOrthoWidth, cOrthoHeight;
		ReadOnlyMeasureHandler mHandler;

		wxDECLARE_EVENT_TABLE();
	};
}