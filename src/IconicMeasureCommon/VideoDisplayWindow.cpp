#include	<IconicMeasureCommon/VideoDisplayWindow.h>
#include    <IconicGpu/IconicLog.h>
#include	<wx/dcclient.h>
#include	<wx/app.h>

using namespace iconic::common;

boost::shared_ptr<wxGLContext> VideoDisplayWindow::gpContext = boost::shared_ptr<wxGLContext>();

wxBEGIN_EVENT_TABLE(VideoDisplayWindow, wxGLCanvas)
    EVT_PAINT(VideoDisplayWindow::OnPaint)
    EVT_SIZE(VideoDisplayWindow::OnSize)
    EVT_LEFT_DOWN(VideoDisplayWindow::OnLeftDown)
wxEND_EVENT_TABLE()

VideoDisplayWindow::VideoDisplayWindow(wxWindow* parent, const wxGLAttributes &canvasAttrs,
                                       unsigned int nDispWidth, unsigned int nDispHeight, unsigned int nTexWidth, unsigned int nTexHeight, bool bUsePbo) :
    wxGLCanvas(parent, canvasAttrs),
    ImageGLBase(nDispWidth, nDispHeight, nTexWidth, nTexHeight, bUsePbo)
{
}

VideoDisplayWindow::~VideoDisplayWindow()
{
}

void VideoDisplayWindow::SetCurrent()
{
    if (!gpContext) {
        gpContext = boost::shared_ptr<wxGLContext>(new wxGLContext(this));
        wxGLCanvas::SetCurrent(*gpContext);
        auto ret = glewInit();
        if (ret != GLEW_OK) {
            LogError("Could not initialize glewInit. Error: %s (code: %d)", static_cast<const unsigned char*>(glewGetErrorString(ret)), static_cast<int>(ret));
        }
        glDisable(GL_LIGHTING);
        glClearColor(0.0, 0.0, 0.0, 1.0f);
        glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
        glEnable(GL_COLOR_MATERIAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        wxLogVerbose(_("OpenGL context created"));
    } else {
        wxGLCanvas::SetCurrent(*gpContext);
    }
}

void VideoDisplayWindow::initGL()
{
    SetCurrent();

    if (cbIsInitializing || cbIsInitialized) {
        return;
    }

    wxLogVerbose("Init GL started");

    auto ret = glewInit();
    if (ret != GLEW_OK) {
        LogError("Could not initialize glewInit. Error: %s (code: %d)", static_cast<const unsigned char*>(glewGetErrorString(ret)), static_cast<int>(ret));
    }

    if (!glewIsSupported("GL_VERSION_1_5 GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object")) {
        wxLogWarning("Error: failed to get minimal extensions for demo");
    }

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    ImageGLBase::initGL();

    // resize viewport
    wxSize clientSize = GetParent()->GetClientSize();
    int w = (int) clientSize.x;
    int h = (int) clientSize.y;
    if (w > (int) nWidth_) {
        w = (int) nWidth_;
    }
    if (h > (int) nHeight_) {
        h = (int) nHeight_;
    }

    SetClientSize(w, h);

    cbIsInitializing = false;
    cbIsInitialized = true;

    return;
}

void VideoDisplayWindow::render(int, bool doRefresh)
{
    if (doRefresh) {
        Refresh(false);
    }
}

bool VideoDisplayWindow::SwapBuffers()
{
    return wxGLCanvas::SwapBuffers();
}

void VideoDisplayWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // This is a dummy, to avoid an endless succession of paint messages.
    // OnPaint handlers must always create a wxPaintDC.
    wxPaintDC dc(this);

    if (!IsShownOnScreen()) {
        return;
    }

    if (cbIsInitializing) {
        return;
    }

    initGL();

    ResetProjectionMode();

    PaintGL();

    wxGLCanvas::SwapBuffers();
}

void VideoDisplayWindow::refresh()
{
    Refresh(false);
}

void VideoDisplayWindow::ResetProjectionMode()
{
    // resize viewport
    GLdouble ow, oh;
    int w, h;
    GetClientSize(&w, &h);

    if (w > h) {
        ow = 0.5;
        oh = (0.5*h) / w;
    } else {
        oh = 0.5;
        ow = (0.5*w) / h;
    }

    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ow, ow, -oh, oh, -1.0, 1.0);
}

void VideoDisplayWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    initGL();

    ResetProjectionMode();

    // Generate paint event without erasing the background.
    Refresh(false);
}

void VideoDisplayWindow::OnLeftDown(wxMouseEvent& e) {
    wxPoint& p = e.GetPosition();
    wxLogStatus("Left click on [%d,%d]", p.x, p.y);
}