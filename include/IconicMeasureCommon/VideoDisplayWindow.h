#pragma once

#include    <IconicVideo/ImageGLBase.h>
#include	<GL/glew.h>
#include	<wx/glcanvas.h>
#include	<boost/shared_ptr.hpp>
#include	<IconicMeasureCommon/Defines.h>

namespace iconic {
namespace common {
//! Image display. Uses NVidia for decoding and wxWidgets OpenGL for display.
/** ImageGLBase is a virtual base class for video decosing with NVidias NVDECODE library.
wxGLCanvas is the implementation that will display the decoded video frames.*/
class ICONIC_MEASURE_COMMON_EXPORT VideoDisplayWindow : public wxGLCanvas, public ImageGLBase {
public:
    //! Constructor
    /**
    \param parent Parent window
    \param canvasAttrs OpenGL attributes
    \param nDispWidth Display width
    \param nDispHeight Display height
    \param nTexWidth Video width
    \param nTexHeight Video height
    \param streamNumber Index to global stream instance, see GpuProcessor::GetStream
    \param ePixelFormat Pixel format to determine number of samples per pixel
    */
    VideoDisplayWindow(wxWindow* parent, const wxGLAttributes &canvasAttrs, unsigned int nDispWidth, unsigned int nDispHeight,
                       unsigned int nTexWidth, unsigned int nTexHeight, bool bUsePbo = true);

    //! Destructor
    virtual ~VideoDisplayWindow();

    //! Handles paint event
    virtual void OnPaint(wxPaintEvent& event);

    //! Called when window is resized
    /**
    * \param event size event
    * \return void
    */
    virtual void OnSize(wxSizeEvent& event);

    /**
     * @brief Demonstrates how to get screen coordinates from mouse click
     * @param e Contains mouse position and more
    */
    void OnLeftDown(wxMouseEvent& e);

    //! Overload from ImageGLBase.
    /** Calls wxGLCanvas::Refresh if refresh==true. Does nothing otherwise. */
    virtual void render(int field_num, bool refresh);

    //! Initialize OpenGL
    virtual void initGL();

    //! Set current OpenGL context
    virtual void SetCurrent();

    //! Calls wxGLCanvas::Refresh
    virtual void refresh();

    //! Swap OpenGL buffers
    virtual bool SwapBuffers();

    //! Static OpenGL context which may be used by other windows as well
    static boost::shared_ptr<wxGLContext> gpContext;

    //! Set default aspect ratio
    void ResetProjectionMode();

protected:
    bool cbInitGL;

    wxDECLARE_EVENT_TABLE();
};
}
}
