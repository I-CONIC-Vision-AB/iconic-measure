#include	<IconicMeasureCommon/ImageCanvas.h>
#include	<IconicMeasureCommon/MeasureEvent.h>
#include	<wx/dcclient.h>
#include	<wx/app.h>
#include	<algorithm>

using namespace iconic;

boost::shared_ptr<wxGLContext> ImageCanvas::gpContext = boost::shared_ptr<wxGLContext>();

wxBEGIN_EVENT_TABLE(ImageCanvas, wxGLCanvas)
EVT_PAINT(ImageCanvas::OnPaint)
EVT_SIZE(ImageCanvas::OnSize)
EVT_MOUSE_EVENTS(ImageCanvas::OnMouse)
EVT_MOUSEWHEEL(ImageCanvas::OnMouseWheel)
wxEND_EVENT_TABLE()

ImageCanvas::ImageCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs,
	unsigned int nDispWidth,
	unsigned int nDispHeight,
	unsigned int nTexWidth,
	unsigned int nTexHeight,
	bool bUsePbo) :
	wxGLCanvas(parent, canvasAttrs),
	ImageGLBase(nDispWidth, nDispHeight, nTexWidth, nTexHeight, bUsePbo),
	cbFitToWindow(true),
	cLastMousePos(),
	cLastClientSize(-1, -1),
	cMouseMode(EMouseMode::MOVE) {}

ImageCanvas::~ImageCanvas() {}

void ImageCanvas::SetCurrent() {
	if (!gpContext) {
		gpContext = boost::shared_ptr<wxGLContext>(new wxGLContext(this));
		wxGLCanvas::SetCurrent(*gpContext);
		auto ret = glewInit();
		if (ret != GLEW_OK) {
			wxLogError("Could not initialize glewInit. Error: %s (code: %d)", wxString(glewGetErrorString(ret)), static_cast<int>(ret));
		}
		glDisable(GL_LIGHTING);
		glClearColor(0.0, 0.0, 0.0, 1.0f);
		glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
		glEnable(GL_COLOR_MATERIAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	else {
		wxGLCanvas::SetCurrent(*gpContext);
	}
}

void ImageCanvas::initGL() {
	SetCurrent();

	if (cbIsInitializing || cbIsInitialized) {
		return;
	}

	if (!glewIsSupported("GL_VERSION_1_5 GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object")) {
		fprintf(stderr, "Error: failed to get minimal extensions for demo\n");
		fprintf(stderr, "This sample requires:\n");
		fprintf(stderr, "  OpenGL version 1.5\n");
		fprintf(stderr, "  GL_ARB_vertex_buffer_object\n");
		fprintf(stderr, "  GL_ARB_pixel_buffer_object\n");
	}

	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	ImageGLBase::initGL();

	// resize viewport
	wxSize clientSize = GetParent()->GetClientSize();
	int w = (int)clientSize.x;
	int h = (int)clientSize.y;
	if (w > (int)nWidth_) {
		w = (int)nWidth_;
	}
	if (h > (int)nHeight_) {
		h = (int)nHeight_;
	}

	cbIsInitializing = false;
	cbIsInitialized = true;

	return;
}

void ImageCanvas::render(int, bool doRefresh) {
	if (doRefresh) {
		Refresh(false);
	}
}

bool ImageCanvas::SwapBuffers() {
	return wxGLCanvas::SwapBuffers();
}

void ImageCanvas::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	// This is a dummy, to avoid an endless succession of paint messages.
	// OnPaint handlers must always create a wxPaintDC.
	wxPaintDC dc(this);

	if (!IsShownOnScreen() || cbIsInitializing) {
		return;
	}

	initGL();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	ResetProjectionMode();

	PaintGL();

	DrawMeasuredGeometries();

	wxGLCanvas::SwapBuffers();
}

void ImageCanvas::DrawMeasuredGeometries() {
	// Draw the measured points
	glPushAttrib(GL_CURRENT_BIT);	// Apply color until pop
	glColor3ub(255, 0, 0);			// Color of geometry
	glPointSize(GetPointSize());	
	glBegin(GL_POINTS);
	for (const boost::compute::float2_& p : cvMeasurements) {
		glVertex2f(p.x, p.y);
	}
	glEnd(); 
	glPopAttrib();	// Resets color
}

void ImageCanvas::OnIdle(wxIdleEvent&) {
	Refresh(false);
}

void ImageCanvas::refresh() {
	Refresh(false);
}

void ImageCanvas::SetFitToWindow(bool set) {
	cbFitToWindow = set;
}

bool ImageCanvas::GetFitToWindow() const {
	return cbFitToWindow;
}

void ImageCanvas::FitToWindow() {
	CenterImage();
	float imageRatio = ((float)nTexWidth_) / nTexHeight_;
	float scale = 1.0f;
	if (imageRatio < 1.0f) {
		// "Portrait" mode so scale by aspect ratio
		wxSize clientSize = GetClientSize();
		scale = ((float)clientSize.y) / clientSize.x;
	}
	SetScale(scale);
}

void ImageCanvas::ResetProjectionMode() {
	// resize viewport
	wxSize clientSize = GetClientSize();
	int w = clientSize.x;
	int h = clientSize.y;

	GLdouble ow, oh;

	ow = 0.5;
	oh = (0.5 * h) / w;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ow, ow, -oh, oh, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cOrthoWidth = ow;
	cOrthoHeight = oh;

	if (cbFitToWindow) {
		FitToWindow();
	}
}

void ImageCanvas::OnSize(wxSizeEvent& WXUNUSED(event)) {
	initGL();

	ResetProjectionMode();

	ProcessBeforePaint(false);

	// Generate paint event without erasing the background.
	Refresh(false);
}

void ImageCanvas::OnMouse(wxMouseEvent& event) {
	switch (cMouseMode) {
	case EMouseMode::MOVE:
		MouseMove(event);
		break;
	case EMouseMode::MEASURE:
		MouseMeasure(event);
		break;
	}
}

void ImageCanvas::MouseMove(wxMouseEvent& event) {
	wxPoint mPos = event.GetPosition();
	wxPoint diff = mPos - cLastMousePos;

	if (event.Dragging() && event.LeftIsDown()) {
		const wxSize& sz = GetClientSize();
		SetFitToWindow(false);
		MoveX((float)diff.x / (float)sz.x);
		MoveY((float)diff.y / (float)sz.x);  // The movement is in percent of the canvas X axis size
		Refresh(false);
	}
	else if (event.RightUp()) {
		MoveX(0);
		MoveY(0);
		SetScale(1);
		SetFitToWindow(true);
		Refresh(false);
	}

	cLastMousePos = mPos;
	event.Skip();  // To not consume all other posible mouse events
}

void ImageCanvas::MouseMeasure(wxMouseEvent& event) {
	if (event.LeftUp()) {
		const wxPoint& screenPoint = event.GetPosition();

		boost::compute::float2_ imagePoint;
		ScreenToCamera(screenPoint, imagePoint.x, imagePoint.y);
		cvMeasurements.push_back(imagePoint);

		MeasureEvent event(MEASURE_POINT, GetId(), imagePoint.x, imagePoint.y, MeasureEvent::EAction::ADDED);
		event.SetEventObject(this);
		ProcessWindowEvent(event);
	}
}

void ImageCanvas::OnMouseWheel(wxMouseEvent& event) {
	const float sensitivity = 0.05f;
	float scale = 1.0f + sensitivity * ((float)event.GetWheelRotation()) / 120.0f;
	SetFitToWindow(false);
	SetScale(GetScale() * scale);
	SetScale(std::max(std::min(GetScale(), 8.0f), 0.125f));
	Refresh(false);
}

void ImageCanvas::SetTitle(char title[]) {
	LogStatus("%s", title[0]);
}

void ImageCanvas::SetMouseMode(EMouseMode mode) {
	cMouseMode = mode;
	switch (cMouseMode) {
	case EMouseMode::MOVE:
		wxLogStatus("Move/zoom in image");
		break;
	case EMouseMode::MEASURE:
		wxLogStatus("Measure in image");
		break;
	}
}

ImageCanvas::EMouseMode ImageCanvas::GetMouseMode() const {
	return cMouseMode;
}

void ImageCanvas::ScreenToCamera(const wxPoint& pt, float& x, float& y) {
	const wxSize screenSize = GetClientSize();
	x = 2.0f * cOrthoWidth / screenSize.x * pt.x - cOrthoWidth;
	y = -2.0f * cOrthoHeight / screenSize.y * pt.y + cOrthoHeight;


	const float scale = GetScale();
	x /= scale;
	y /= scale;

	boost::compute::float2_ translation;
	GetTranslation(translation.x, translation.y);
	x -= translation.x;
	y += translation.y;
}
