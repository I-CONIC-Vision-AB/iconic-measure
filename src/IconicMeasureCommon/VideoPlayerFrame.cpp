#include	<IconicMeasureCommon/Defines.h>
#include	<IconicMeasureCommon/VideoPlayerFrame.h>
#include	<IconicMeasureCommon/Geometry.h> 
#include	<IconicMeasureCommon/OpenCLGrid.h>
#include	<IconicMeasureCommon/ColorBox.h>
#include	<IconicMeasureCommon/Shape.h>
#include	<wx/filename.h>
#include	<wx/aboutdlg.h>
#include	<wx/versioninfo.h>
#include	<wx/numdlg.h>
#include    <wx/textdlg.h>
#include    <wx/config.h>
#include    <wx/splitter.h>
#include	<wx/textfile.h>
#include	<boost/make_shared.hpp>
#include	<IconicGpu/GpuContext.h>
#include    <IconicGpu/wxMACAddressUtility.h>
#include    <IconicGpu/IconicLog.h>
#include	<wx/tokenzr.h>
#include	<iostream>
#include	<fstream>
#include	<boost/geometry/io/wkt/wkt.hpp>
#include	<boost/geometry.hpp>
#include	<string>



using namespace iconic;
using namespace iconic::common;

// Event handlers
wxBEGIN_EVENT_TABLE(VideoPlayerFrame, wxFrame)
EVT_MENU(wxID_OPEN, VideoPlayerFrame::OnOpen)
EVT_MENU(ID_OPEN_FOLDER, VideoPlayerFrame::OnOpenFolder)
EVT_MENU(ID_NEXT, VideoPlayerFrame::OnNextImage)
EVT_MENU(wxID_SAVE, VideoPlayerFrame::OnSave)
EVT_MENU(wxID_EXIT, VideoPlayerFrame::OnQuit)
EVT_MENU(wxID_ABOUT, VideoPlayerFrame::OnAbout)
EVT_MENU(ID_PAUSE, VideoPlayerFrame::OnPause)
EVT_MENU(ID_FULLSCREEN, VideoPlayerFrame::OnFullscreen)
EVT_MENU(ID_OPENCL_CAPS, VideoPlayerFrame::OnOpenCLCapabilities)
EVT_MENU(ID_VIDEO_USE_TIMER, VideoPlayerFrame::OnUseTimer)
EVT_MENU(ID_VIDEO_SET_FPS, VideoPlayerFrame::OnSetFrameRate)
EVT_MENU(ID_VIDEO_SHOW_LOG, VideoPlayerFrame::OnShowLog)
EVT_MENU(ID_VIDEO_DECODER, VideoPlayerFrame::OnVideoDecoder)
EVT_MENU(ID_MOUSE_MODE, VideoPlayerFrame::OnMouseMode)
EVT_MENU(ID_TOOLBAR_MOVE, VideoPlayerFrame::OnToolbarPress)
EVT_MENU(ID_TOOLBAR_LINE, VideoPlayerFrame::OnToolbarPress)
EVT_MENU(ID_TOOLBAR_POLYGON, VideoPlayerFrame::OnToolbarPress)
EVT_MENU(ID_TOOLBAR_POINT, VideoPlayerFrame::OnToolbarPress)
EVT_MENU(ID_TOOLBAR_DELETE, VideoPlayerFrame::OnToolbarPress)
EVT_MENU(ID_TESSELATE_DUMMY_EXAMPLE, VideoPlayerFrame::OnDrawTesselatedPolygon)
EVT_MENU(ID_CLEAR_ALL_SHAPES, VideoPlayerFrame::OnDeleteAllShapes)
EVT_TOOL(ID_TOOLBAR_SIDEPANEL, VideoPlayerFrame::OnToolbarCheck)
EVT_MENU(ID_LOAD_WKT, VideoPlayerFrame::OnLoadMeasurements)
EVT_UPDATE_UI(ID_MOUSE_MODE, VideoPlayerFrame::OnMouseModeUpdate)
EVT_UPDATE_UI(ID_PAUSE, VideoPlayerFrame::OnUpdatePause)
EVT_UPDATE_UI(ID_FULLSCREEN, VideoPlayerFrame::OnUpdateFullscreen)
EVT_UPDATE_UI(ID_VIDEO_USE_TIMER, VideoPlayerFrame::OnUpdateUseTimer)
EVT_UPDATE_UI(ID_VIDEO_DECODER, VideoPlayerFrame::OnUpdateVideoDecoder)
EVT_IDLE(VideoPlayerFrame::OnIdle)
EVT_TIMER(ID_VIDEO_TIMER, VideoPlayerFrame::OnTimer)
EVT_CLOSE(VideoPlayerFrame::OnClose)
wxEND_EVENT_TABLE()

const wxWindowID ID_TOOLBAR_TEXT = wxNewId();

VideoPlayerFrame::VideoPlayerFrame(wxString const& title, boost::shared_ptr<wxVersionInfo> pVersionInfo, int streamNumber, bool bImmediateRefresh, MeasureHandlerPtr pHandler)
	: wxFrame(NULL, wxID_ANY, title),
	cpImageCanvas(NULL),
	cpVersionInfo(pVersionInfo),
	cStreamNumber(streamNumber),
	cbRefresh(bImmediateRefresh),
	cbPause(true),
	cbUseTimer(false),
	cFrameRate(-1.0),
	cOriginalFrameRate(-1.0),
	cbLoop(false),
	cpLogTextCtrl(nullptr),
	cpWindowLog(nullptr),
	cpDefaultLog(nullptr),
	csVideoDecoderName("IconicVideoNV8"),
	cbIsOpened(false),
	cbFastForward(false),
	cpHandler(pHandler) {
	CreateStatusBar(1);
	SetStatusText("I-CONIC Measure");

	CreateMenu();
	CreateLayout();


	Maximize();

	cTimer.SetOwner(this, ID_VIDEO_TIMER);
}

VideoPlayerFrame::~VideoPlayerFrame() {
	if (cpDecoder) {
		cpDecoder->Stop();
	}
	cpDecoder = VideoDecoderPtr();
}

void VideoPlayerFrame::CreateLayout() {
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
	sizer->Add(splitter, 6, wxEXPAND | wxALL);

	cSide_panel = new SidePanel(splitter);
	cSide_panel->SetBackgroundColour(wxColour(180, 230, 230));

	// This can't be the best solution but it looks better for now, just to have a split screen before opening a video
	cHolder_panel = new wxPanel(splitter, wxID_ANY);
	cHolder_panel->SetBackgroundColour(wxColor(100, 100, 100));

	splitter->SetMinimumPaneSize(200);
	splitter->SplitVertically(cHolder_panel, cSide_panel, -400);

	splitter->SetSashGravity(0.5);
	SetSizer(sizer);
	Centre();
}

void VideoPlayerFrame::CreateMenu() {
	wxMenuBar* menuBar = new wxMenuBar();

	wxMenu* fileMenu = new wxMenu;
	wxMenu* openMenu = new wxMenu;
	openMenu->Append(wxID_OPEN, "&Open...\tCtrl+O", "Open video file");
	openMenu->Append(ID_OPEN_FOLDER, "&Open folder\tCtrl+Alt+O", "Open still images in directory");
	fileMenu->AppendSubMenu(openMenu, _("Open"), _("Open video, folder or network"));
	fileMenu->Append(wxID_SAVE, _("Save...\tCtrl+S"), _("Save decoded frames to file or stream"));
	fileMenu->Append(ID_LOAD_WKT, _("Load measurements"), _("Load measurements from wkt file"));
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
	menuBar->Append(fileMenu, "&File");

	wxMenu* viewMenu = new wxMenu;
	viewMenu->AppendCheckItem(ID_PAUSE, _("Pause\tSPACE"), _("Pause/play video"))->Check(cbPause);
	viewMenu->Append(ID_NEXT, _("Next frame\tTAB"), _("Go to next frame/image"));
	viewMenu->AppendCheckItem(ID_MOUSE_MODE, _("Measure"), _("Toggle measure mode"));
	viewMenu->Append(ID_VIDEO_SET_FPS, _("Frame rate...\tF4"), _("Set video playback frame rate"));
	viewMenu->AppendCheckItem(ID_VIDEO_USE_TIMER, _("Max speed\tF3"), _("Toggle max or natural video frame rate"))->Check(false);
	viewMenu->AppendSeparator();
	viewMenu->AppendCheckItem(ID_FULLSCREEN, _("Fullscreen\tF11"), _("Toggle full screen mode"))->Check(false);
	viewMenu->AppendSeparator();
	viewMenu->Append(ID_TESSELATE_DUMMY_EXAMPLE, _("Tesselate test..."), _("Test drawing concave polygon with hole.\nTo be removed!"));
	viewMenu->Append(ID_CLEAR_ALL_SHAPES, _("Clear shapes"), _("Delete all shapes currently created."));
	menuBar->Append(viewMenu, "&View");

	wxMenu* settingsMenu = new wxMenu;
	settingsMenu->Append(ID_VIDEO_DECODER, _("Set video decoder...\tCtrl+V"), _("Select which video decoder to use."));
	settingsMenu->AppendCheckItem(ID_VIDEO_SHOW_LOG, _("&Log to window\tCtrl+L"), _("Show log window"))->Check(false);
	menuBar->Append(settingsMenu, "&Settings");

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(ID_OPENCL_CAPS, "&OpenCL\tF2", "Show OpenCL capabilites on this platform");
	helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
	menuBar->Append(helpMenu, "&Help");

	SetMenuBar(menuBar);

	// Toolbar
	cToolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_FLAT | wxTB_TEXT);

	cToolbar->SetToolSeparation(10);

	cToolbar->SetToolBitmapSize(wxSize(32, 32));

	wxBitmap moveBmp(wxT("./img/move.png"), wxBITMAP_TYPE_PNG);
	wxBitmap pointBmp(wxT("./img/point.png"), wxBITMAP_TYPE_PNG);
	wxBitmap lineBmp(wxT("./img/line.png"), wxBITMAP_TYPE_PNG);
	wxBitmap polygonBmp(wxT("./img/polygon.png"), wxBITMAP_TYPE_PNG);
	wxBitmap deleteBmp(wxT("./img/delete.png"), wxBITMAP_TYPE_PNG);
	wxBitmap sidepanelBmp(wxT("./img/sidepanel.png"), wxBITMAP_TYPE_PNG);

	cToolbar->AddRadioTool(ID_TOOLBAR_MOVE, _("Move"), moveBmp, wxNullBitmap, _("Move the canvas \tM"), _("Allows movement of the canvas."));
	cToolbar->SetToolLongHelp(ID_TOOLBAR_MOVE, _("Move tool"));

	cToolbar->AddRadioTool(ID_TOOLBAR_POINT, _("Point"), pointBmp, wxNullBitmap, _("Draw point \tI"), _("Allows placing of points on the canvas."));
	cToolbar->SetToolLongHelp(ID_TOOLBAR_POINT, _("Point tool"));

	cToolbar->AddRadioTool(ID_TOOLBAR_LINE, _("Line"), lineBmp, wxNullBitmap, _("Draw line \tL"), _("Allows drawing of line segments on the canvas."));
	cToolbar->SetToolLongHelp(ID_TOOLBAR_LINE, _("Line tool"));

	cToolbar->AddRadioTool(ID_TOOLBAR_POLYGON, _("Polygon"), polygonBmp, wxNullBitmap, _("Draw polygon \tP"), _("Allows drawing of polygons on the canvas."));
	cToolbar->SetToolLongHelp(ID_TOOLBAR_POLYGON, _("Polygon tool"));

	cToolbar->AddSeparator();

	cToolbar->AddTool(ID_TOOLBAR_DELETE, _("Delete"), deleteBmp, _("Delete selected shape. \tDELETE"));
	cToolbar->SetToolLongHelp(ID_TOOLBAR_DELETE, _("Delete button"));

	cToolbar->AddSeparator();

	wxToolBarToolBase* sidepanelTool = cToolbar->AddCheckTool(ID_TOOLBAR_SIDEPANEL, _("Show side panel"), sidepanelBmp, wxNullBitmap, _("Show or hide the side panel containing measured objects."));
	sidepanelTool->Toggle(true); // Set the default state of the button to be pressed

	cToolbar->AddSeparator();

	cColorBox = new ColorBox(cToolbar, wxID_ANY, wxDefaultPosition, wxSize(32, 32), wxBORDER_NONE);
	cColorBox->SetColor(wxColor(238, 238, 238));
	cToolbar->AddControl(cColorBox);

	cToolbar->AddControl(new wxStaticText(cToolbar, ID_TOOLBAR_TEXT, "Selected shape: none selected"));

	cToolbar->Realize();

	// table of keyboard shortcuts
	wxAcceleratorEntry entries[] = {
		wxAcceleratorEntry(wxACCEL_NORMAL, (int)'M', ID_TOOLBAR_MOVE),
		wxAcceleratorEntry(wxACCEL_NORMAL, (int)'I', ID_TOOLBAR_POINT),
		wxAcceleratorEntry(wxACCEL_NORMAL, (int)'L', ID_TOOLBAR_LINE),
		wxAcceleratorEntry(wxACCEL_NORMAL, (int)'P', ID_TOOLBAR_POLYGON),
		wxAcceleratorEntry(wxACCEL_NORMAL, WXK_DELETE, ID_TOOLBAR_DELETE),
	};
	wxAcceleratorTable accelTable(WXSIZEOF(entries), entries);
	SetAcceleratorTable(accelTable);
}

void VideoPlayerFrame::OnOpen(wxCommandEvent& WXUNUSED(event)) {
	wxString filename;
	filename = ::wxFileSelector(_("Video file"), wxEmptyString, wxEmptyString, wxEmptyString,
		wxString("mp4|*.mp4|avi|*.avi|mkv|*.mkv|mov|*.mov|ts|*.ts|mpg|*.mpg|All Files|*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST /*| wxFD_PREVIEW*/, this);
	if (filename.IsEmpty()) {
		return;
	}
	OpenVideo(filename);
}

void VideoPlayerFrame::OnClose(wxCloseEvent& event) {
	if (cpHandler)
		cpHandler->ClearShapes();

	Destroy();
}

void VideoPlayerFrame::OnOpenFolder(wxCommandEvent& WXUNUSED(event)) {
	csVideoDecoderName = wxString("IconicVideoFolder");
	wxString dir = wxDirSelector(_("Select image folder"), wxEmptyString, 536877120L, wxDefaultPosition, this);
	if (dir.IsEmpty()) {
		return;
	}
	OpenVideo(dir);
}

void VideoPlayerFrame::OnSave(wxCommandEvent& WXUNUSED(e))
{
	wxFileDialog saveFileDialog(this, _("Save wkt file"), "", "",
							"WKT files (*.wkt)|*.wkt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() != wxID_OK)
		return;     // the user changed idea...

	std::ofstream SaveFile(saveFileDialog.GetPath().mb_str());

	std::string wktOutput;
	cpHandler->GetWKT(wktOutput);
	SaveFile << wktOutput << std::endl;

	SaveFile.close();
}

void VideoPlayerFrame::OnLoadMeasurements(wxCommandEvent& WXUNUSED(e)) {

	wxString        file;
	wxFileDialog    fdlog(this, _("Load wkt file"), "", "",
		"WKT files (*.wkt)|*.wkt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (fdlog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = fdlog.GetPath();

	wxString        str;

	// open the file
	wxTextFile      tfile;
	tfile.Open(file);

	// read the first line
	DataUpdateEvent updateEvent(GetId());
	str = tfile.GetFirstLine();
	if (cpHandler->LoadWKT(str, updateEvent)) {
		updateEvent.SetEventObject(this);
		ProcessWindowEvent(updateEvent);
	}

	
	while (!tfile.Eof())
	{
		DataUpdateEvent updateEvent(GetId());
		str = tfile.GetNextLine();
		if (cpHandler->LoadWKT(str, updateEvent)) {
			updateEvent.SetEventObject(this);
			ProcessWindowEvent(updateEvent);
		}
	}
}

wxString VideoPlayerFrame::GetVideoFileName() const {
	return cFileName;
}

void VideoPlayerFrame::OpenVideo(wxString filename) {
	if (filename.IsEmpty()) {
		return;
	}

	cFileName = filename;
	char file[256];
	strcpy(&(file[0]), filename.char_str());

	if (cpImageCanvas) {
		cpImageCanvas->Destroy();
	}
	if (cpDecoder) {
		cpDecoder->Stop();
	}
	if (!CreateDecoder()) {
		wxLogError(_("Could not create video decoder"));
		return;
	}

	if (!filename.IsEmpty()) {
		wxFileName fn(filename);
		SetStatusText(fn.GetFullName());

		if (!cpDecoder->LoadVideo(file)) {
			wxLogError(_("Could not load video"));
			return;
		}
	}
	// Create OpenGL window
	wxGLAttributes vAttrs;
	wxSize s = GetSize();
	vAttrs.PlatformDefaults().Defaults().EndList();
	cpImageCanvas = new ImageCanvas(splitter, vAttrs, s.x, s.y, cpDecoder->GetVideoWidth(), cpDecoder->GetVideoHeight(), cpDecoder->UsePbo());

	Bind(MEASURE_POINT, &VideoPlayerFrame::OnMeasuredPoint, this, cpImageCanvas->GetId());
	if (cpHandler)
		Bind(DRAW_SHAPES, &MeasureHandler::OnDrawShapes, cpHandler.get(), cpImageCanvas->GetId());
	Bind(DATA_UPDATE, &SidePanel::Update, cSide_panel, GetId());
	Bind(DATA_UPDATE, &VideoPlayerFrame::UpdateToolbarMeasurement, this, GetId());


	// Decoding starts here. Some frames are enqueued. They need to be dequeued in order to traverse the entire video.
	// Dequeue is done in DecodeFrame, see OnIdle
	cpDecoder->Start(cpImageCanvas);
	wxLogVerbose(_("Decoder started"));

	wxWindow* holder = splitter->GetWindow1();
	splitter->ReplaceWindow(holder, cpImageCanvas);
	holder->Destroy();

	Layout();

	cbIsOpened = true;

	Show();

	SetMipMap(true);
	SetInterpolation(true);
	cpImageCanvas->SetPointSize(3.0f);

	int processFlag = 7; // EOperation::texture | EOperation::pyramid | EOperation::gpuimage;
	cpImageCanvas->ProcessBeforePaint(processFlag);

	wxLogVerbose(_("Layout done"));
}

void VideoPlayerFrame::OnIdle(wxIdleEvent& e) {
	if (cbPause || cbUseTimer) {
		// Do not trigger new frames to be drawn
		return;
	}
	GetDecodedFrame();
	if (!cpDecoder->IsDone()) {
		// Keep sending idle events. The events will be queued and will not block other events, e.g. from user.
		e.RequestMore(true);
	} else {
		if (cbLoop) {
			char file[256];
			strcpy(&(file[0]), cFileName.char_str());

			if (!cpDecoder->LoadVideo(file)) {
				e.RequestMore(false);
			}

			cpDecoder->Start(cpImageCanvas);
			e.RequestMore(true);
		} else {
			LogStatus("Video displayed in %ss ", cClockTimer.format((short)6, "%w"));
			cClockTimer.stop();
			e.RequestMore(false);
		}
	}
}

void VideoPlayerFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(true);
}

void VideoPlayerFrame::OnFullscreen(wxCommandEvent& e) {
	ShowFullScreen(e.IsChecked());
}

void VideoPlayerFrame::StartTimer() {
	if (!cpDecoder) {
		return;
	}
	double frameRate = cpDecoder->GetFrameTime();
	if (cFrameRate != -1.0 && cFrameRate != frameRate) {
		frameRate = cFrameRate;
		cpDecoder->SetFrameTimeHint(frameRate);
	}
	if (frameRate <= 0.0) {
		frameRate = 1.0 / 30.0;
	}
	int msec = (int)(frameRate * 1000.0 + 0.5);
	if (!cTimer.IsRunning()) {
		cTimer.Start(msec);
	}
}

void VideoPlayerFrame::OnPause(wxCommandEvent& e) {
	if (!cpDecoder || !cpImageCanvas) {
		return;
	}
	cbPause = e.IsChecked();
	if (cbUseTimer) {
		if (cbPause) {
			cTimer.Stop();
		} else {
			StartTimer();
		}
	} else if (!cbPause) {
		// Paints the image and triggers idle event
		cClockTimer.start();
		cpImageCanvas->Refresh();
	}
}

void VideoPlayerFrame::OnNextImage(wxCommandEvent& WXUNUSED(event)) {
	if (!cpDecoder || !cpImageCanvas) {
		return;
	}

	GetDecodedFrame();

	if (cpHandler) {
		if (!cpHandler->Parse()) {
			wxLogWarning("No depth map or camera found");
			return;
		}
	}
}

void VideoPlayerFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxIcon icon(wxString("aaaaaaaa"), wxBITMAP_TYPE_ICO_RESOURCE, 64, 64);
	wxAboutDialogInfo info;

	info.SetVersion(cpVersionInfo->GetVersionString());
	info.SetDescription(cpVersionInfo->GetDescription());
	info.SetWebSite(wxString("http://www.i-conic.eu"), wxString("I-CONIC Vision"));
	info.SetCopyright(cpVersionInfo->GetCopyright());
	info.SetIcon(icon);
	info.AddDeveloper(wxString("H\u00E5kan Wiman"));
	info.AddDeveloper(wxString("\"Group 20, PVK, KTH, 2023\""));

	wxAboutBox(info, this);
}

void VideoPlayerFrame::OnOpenCLCapabilities(wxCommandEvent& WXUNUSED(event)) {
	OpenCLDialog* pDialog = new OpenCLDialog(this);
	pDialog->ShowModal();
	pDialog->Destroy();
}

void VideoPlayerFrame::OnUseTimer(wxCommandEvent& e) {
	cbUseTimer = !e.IsChecked();
	if (cbUseTimer) {
		StartTimer();
	} else if (cpImageCanvas) {
		cpImageCanvas->Refresh();
	}
}

void VideoPlayerFrame::OnSetFrameRate(wxCommandEvent& WXUNUSED(event)) {
	double secPerFrame;
	long fps = 30;
	if (cpDecoder) {
		secPerFrame = cpDecoder->GetFrameTime();
	} else if (cFrameRate != -1.0) {
		secPerFrame = cFrameRate;
	}
	if (secPerFrame <= 0) {
		secPerFrame = 1.0 / 30;
	}
	fps = (long)(1.0 / secPerFrame + 0.5);
	fps = wxGetNumberFromUser(_("Frame rate"), _("Enter number of frames per second (fps)"), _("Iconic Video"), fps, 1, 1000, this);
	if (fps > 0) {
		cFrameRate = 1.0 / fps;
		if (cpDecoder) {
			cpDecoder->SetFrameTimeHint(cFrameRate);
		}
		if (cTimer.IsRunning()) {
			cTimer.Stop();
			StartTimer();
		}
	}
}

void VideoPlayerFrame::OnShowLog(wxCommandEvent& e) {
	if (!cpLogTextCtrl) {
		//// Make a textctrl for logging
		cpLogTextCtrl = new wxTextCtrl(this, wxID_ANY, _("Log\n"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

		GetSizer()->Add(cpLogTextCtrl, 1, wxEXPAND);

		cpDefaultLog = wxLog::GetActiveTarget();
		wxLogLevel logLevel = cpDefaultLog->GetLogLevel();
		cpDefaultLog->SetLogLevel(wxLOG_Message);
		logLevel = cpDefaultLog->GetLogLevel();
		cpWindowLog = new wxLogTextCtrl(cpLogTextCtrl);
		cpWindowLog->SetLogLevel(wxLOG_Info);
	}
	if (e.IsChecked()) {
		wxLog::SetActiveTarget(cpWindowLog);
		cpDefaultLog->SetLogLevel(wxLOG_Info);
		cpWindowLog->SetVerbose();
		cpLogTextCtrl->Show();
	} else {
		wxLog::SetActiveTarget(cpDefaultLog);
		cpDefaultLog->SetLogLevel(wxLOG_Message);
		cpDefaultLog->SetVerbose(false);
		cpLogTextCtrl->Hide();
	}
	Layout();
}

void VideoPlayerFrame::SetInterpolation(bool set) {
	if (cpImageCanvas) {
		glFinish();
		cpImageCanvas->setUseNearest(!set);
		cpImageCanvas->refresh();
	}
	LogStatus("Interpolation is %s", set ? "linear" : "nearest neighbour");
}

void VideoPlayerFrame::SetMipMap(bool set) {
	if (cpImageCanvas) {
		glFinish();
		cpImageCanvas->setUseMipMaps(set);
		cpImageCanvas->refresh();
	}
	LogStatus("Mipmap generation is %s", set ? "on" : "off");
}

void VideoPlayerFrame::GetDecodedFrame() {
	if (!cpDecoder || !cpImageCanvas) {
		return;
	}

	if (cpDecoder->IsDone()) {
		if (cbLoop) {
			char file[256];
			strcpy(&(file[0]), cFileName.char_str());

			if (!cpDecoder->LoadVideo(file)) {
			} else {
				cpDecoder->Start(cpImageCanvas);
			}
		} else {
			return;
		}
	}
	bool bFramesDecoded = false;
	cpDecoder->DecodeFrame(cpImageCanvas, true, cbRefresh && !cbFastForward, bFramesDecoded);
	if (bFramesDecoded) {
		gpu::ImagePropertyPtr pProperties = cpDecoder->GetProperties();
		if (pProperties) {
			wxString sFileName(wxEmptyString);
			sFileName = pProperties->Get(gpu::ImageProperty::EName::FILENAME, sFileName);
			// ToDo: Look for depth map with same name
		}

		if (!cbFastForward) {
			ProcessDecodedFrame();
			if (!cbRefresh) {
				// Refresh was not done while decoding and we are not fast forwarding so do it now after handling decoded frame.
				cpImageCanvas->Refresh();
			}
		}

		cbFastForward = false;
		cpDecoder->SetFastForward(cbFastForward);
		cpImageCanvas->SetFastForward(cbFastForward);
	}
}

void VideoPlayerFrame::ProcessDecodedFrame() {
	// Intentionally empty. No additional processing.
}

int VideoPlayerFrame::GetStreamNumber() const {
	return cStreamNumber;
}

void VideoPlayerFrame::OnTimer(wxTimerEvent& WXUNUSED(e)) {
	GetDecodedFrame();
	if (cpDecoder->IsDone()) {
		cTimer.Stop();
	}
}

void VideoPlayerFrame::OnUpdatePause(wxUpdateUIEvent& e) {
	e.Check(cbPause);
}

void VideoPlayerFrame::OnUpdateFullscreen(wxUpdateUIEvent& e) {
	e.Check(IsFullScreen());
}

void VideoPlayerFrame::OnUpdateUseTimer(wxUpdateUIEvent& e) {
	e.Check(!cbUseTimer);
}

void VideoPlayerFrame::OnVideoDecoder(wxCommandEvent& WXUNUSED(event)) {
	wxArrayString choices;
	std::map<GpuVideoDecoder::EIconicVideoDecoder, wxString> mDecoders = GpuVideoDecoder::GetAvailableDecoders();
	std::map<GpuVideoDecoder::EIconicVideoDecoder, wxString>::iterator it;
	for (it = mDecoders.begin(); it != mDecoders.end(); ++it) {
		choices.Add(it->second);
	}
	csVideoDecoderName = wxGetSingleChoice(_("Select video decoder"), _("Video decoder"), choices, 0, this);
}

bool VideoPlayerFrame::CreateDecoder() {
	if (csVideoDecoderName.IsEmpty()) {
		wxLogError(_("No video decoder is selected or found"));
		return false;
	}
	cpDecoder = VideoDecoderPtr(GpuVideoDecoder::CreateDecoder(csVideoDecoderName.char_str()));
	if (!cpDecoder) {
		wxLogError("failed to create video decoder");
		return false;
	}
	if (cpHandler) {
		cpDecoder->SetMetaDataHandler(cpHandler);
	}

	return true;
}

void VideoPlayerFrame::OnUpdateVideoDecoder(wxUpdateUIEvent& e) {
	e.Enable(!cbIsOpened);
}

void VideoPlayerFrame::SetMouseMode(ImageCanvas::EMouseMode mode) {
	cpImageCanvas->SetMouseMode(mode);
	switch (mode) {
	case ImageCanvas::EMouseMode::MOVE:
		wxLogStatus("Move/zoom in image");
		break;
	case ImageCanvas::EMouseMode::MEASURE:
		wxLogStatus("Measure in image");
		break;
	}
}

void VideoPlayerFrame::OnToolbarPress(wxCommandEvent& e) {
	if (!cpHandler) {
		wxLogError(_("No measurement handler"));
		return;
	}
	// Only finish measurement if currently in measure mode
	if (GetMouseMode() == ImageCanvas::EMouseMode::MEASURE)
		cpHandler->HandleFinishedMeasurement(false);

	switch (e.GetId()) {
	case ID_TOOLBAR_MOVE:
		cToolbar->ToggleTool(ID_TOOLBAR_MOVE, true);
		SetMouseMode(ImageCanvas::EMouseMode::MOVE);
		break;
	case ID_TOOLBAR_LINE:
		cToolbar->ToggleTool(ID_TOOLBAR_LINE, true);
		SetMouseMode(ImageCanvas::EMouseMode::MEASURE);
		cpHandler->InstantiateNewShape(iconic::ShapeType::LineType);
		break;
	case ID_TOOLBAR_POLYGON:
		cToolbar->ToggleTool(ID_TOOLBAR_POLYGON, true);
		SetMouseMode(ImageCanvas::EMouseMode::MEASURE);
		cpHandler->InstantiateNewShape(iconic::ShapeType::PolygonType);
		break;
	case ID_TOOLBAR_POINT:
		cToolbar->ToggleTool(ID_TOOLBAR_POINT, true);
		SetMouseMode(ImageCanvas::EMouseMode::MEASURE);
		cpHandler->InstantiateNewShape(iconic::ShapeType::PointType);
		break;
	case ID_TOOLBAR_DELETE:
		int indexToDelete = cpHandler->DeleteSelectedShape();
		if (indexToDelete < 0) break;
		DataUpdateEvent updateEvent(GetId(), indexToDelete);
		updateEvent.SetEventObject(this);
		ProcessWindowEvent(updateEvent);
		break;
	}
	cpImageCanvas->refresh();
	cToolbar->Realize();
}

ImageCanvas::EMouseMode VideoPlayerFrame::GetMouseMode() const {
	if (!cpImageCanvas) {
		return ImageCanvas::EMouseMode::MOVE;
	}
	return cpImageCanvas->GetMouseMode();
}

void VideoPlayerFrame::OnMouseMode(wxCommandEvent& WXUNUSED(e)) {
	// Toggle MOVE->MEASURE or v/v
	ImageCanvas::EMouseMode mode = GetMouseMode();
	switch (mode) {
	case ImageCanvas::EMouseMode::MOVE:
		SetMouseMode(ImageCanvas::EMouseMode::MEASURE);
		break;
	case ImageCanvas::EMouseMode::MEASURE:
		SetMouseMode(ImageCanvas::EMouseMode::MOVE);
		break;
	}
}

void VideoPlayerFrame::OnMouseModeUpdate(wxUpdateUIEvent& e) {
	e.Check(GetMouseMode() == ImageCanvas::EMouseMode::MEASURE);
}

void VideoPlayerFrame::SetToolbarText(wxString text) {
	cToolbar->FindControl(ID_TOOLBAR_TEXT)->SetLabel(text);
}

void VideoPlayerFrame::UpdateToolbarMeasurement(DataUpdateEvent& e) {

	if (e.IsDeletionEvent()) {
		SetToolbarText("Selected shape: none selected");
		cColorBox->SetColor(wxColor(238, 238, 238));
		e.Skip();
		return;
	}
	ShapePtr shape = e.GetShape();

	cColorBox->SetColor(shape->GetColor());

	switch (shape->GetType()) {
	case iconic::ShapeType::PointType:
	{
		Geometry::Point3D p;
		shape->GetCoordinate(p);
		SetToolbarText(wxString::Format("Selected point: x = %.4f, y = %.4f, z = %.4f", p.get<0>(), p.get<1>(), p.get<2>()));
		break;
	}
	case iconic::ShapeType::LineType:
	{
		SetToolbarText(wxString::Format("Selected line: length = %.4f", shape->GetLength()));
		break;
	}
	case iconic::ShapeType::PolygonType:
	{
		SetToolbarText(wxString::Format("Selected polygon: perimeter = %.4f, area = %.4f, volume = %.4f", shape->GetLength(), shape->GetArea(), shape->GetVolume()));
		break;
	}
	}
	e.Skip();
}

void VideoPlayerFrame::OnMeasuredPoint(MeasureEvent& e) {
	if (!cpHandler) {
		wxLogError(_("No measurement handler"));
		return;
	}
	float x, y;
	e.GetPoint(x, y);

	DataUpdateEvent updateEvent(GetId());
	bool callEvent = false;

	switch (e.GetAction()) {
	case MeasureEvent::EAction::MOVED:
	{
		const Geometry::Point imagePt(static_cast<double>(x), static_cast<double>(y));
		callEvent = cpHandler->ModifySelectedShape(imagePt, e.GetAction(), updateEvent);
		break;
	}
	case MeasureEvent::EAction::SELECTED:
	{
		const Geometry::Point imagePt(static_cast<double>(x), static_cast<double>(y));

		callEvent = cpHandler->ModifySelectedShape(imagePt, e.GetAction(), updateEvent);

		break;
	}
	case MeasureEvent::EAction::ADDED:
	{
		// Sample code transforming the measured point to object space
		// ToDo: You probably want to either create a polygon or other geometry in the handler with this as first point
		// or append this point to an already created active polygon
		const Geometry::Point imagePt(static_cast<double>(x), static_cast<double>(y));
		callEvent = cpHandler->ModifySelectedShape(imagePt, e.GetAction(), updateEvent);

		break;
	}
	case MeasureEvent::EAction::SELECT:
	{
		const ShapeType type = cpHandler->SelectShapeFromCoordinates(Geometry::Point(x, y));
		if (type == ShapeType::None) {
			SetToolbarText("Selected shape: none selected");
			cColorBox->SetColor(wxColor(238, 238, 238));
		}

		break;
	}
	case MeasureEvent::EAction::SELECTandEDIT:
	{
		const ShapeType type = cpHandler->SelectShapeFromCoordinates(Geometry::Point(x, y));
		if (type == ShapeType::None) {
			SetToolbarText("Selected shape: none selected");
			cColorBox->SetColor(wxColor(238, 238, 238));
		} else {
			switch (type) {
			case ShapeType::PointType:
				cToolbar->ToggleTool(ID_TOOLBAR_POINT, true);
				break;
			case ShapeType::LineType:
				cToolbar->ToggleTool(ID_TOOLBAR_LINE, true);
				break;
			case ShapeType::PolygonType:
				cToolbar->ToggleTool(ID_TOOLBAR_POLYGON, true);
				break;
			}
			SetMouseMode(ImageCanvas::EMouseMode::MEASURE);
		}
		break;
	}
	case MeasureEvent::EAction::FINISHED:
	{
		cpHandler->HandleFinishedMeasurement();
		break;
	}
	}

	if (callEvent) {
		updateEvent.SetEventObject(this);
		ProcessWindowEvent(updateEvent);
	}

	cpImageCanvas->Refresh();
}

void VideoPlayerFrame::OnDrawTesselatedPolygon(wxCommandEvent& e) {
	if (!cpHandler) {
		wxLogError(_("No measurement handler"));
		return;
	}
	Geometry::PolygonPtr pPolygon = boost::make_shared<Geometry::Polygon>();

	// Create a square with concave left and right sides
	pPolygon->outer() = {
		Geometry::Point(-0.25f,-0.25f),
		Geometry::Point(-0.15f,0.0f), // concave 1
		Geometry::Point(-0.25f,0.25f),
		Geometry::Point(0.25f,0.25f),
		Geometry::Point(0.15f,0.0f), // concave 2
		Geometry::Point(0.25f,-0.25f)
	};

	// Create a smaller square inside outer square. Let it run counter-clockwise to define it as interior
	boost::geometry::model::ring<Geometry::Point, false> hole = {
		Geometry::Point(-0.1f,-0.1f),
		Geometry::Point(0.1f,-0.1f),
		Geometry::Point(0.1f,0.1f),
		Geometry::Point(-0.1f,0.1f)
	};

	// Add the small square to inner polygons. This makes it a hole
	pPolygon->inners().push_back(hole);

	boost::shared_ptr<PolygonShape> pPolygonShape(new PolygonShape(pPolygon));
	pPolygonShape->SetDrawMode(true, false, false); // Toggle the flags to show/hide transparency, tesselated lines, boundary points
	cpHandler->AddImagePolygon(pPolygon); // Does not add an object polygon. This is just an example...
	Refresh();
}

void VideoPlayerFrame::OnDeleteAllShapes(wxCommandEvent& e) {
	cpHandler->DeleteAllShapes();

	DataUpdateEvent updateEvent(GetId(), -1);
	updateEvent.SetEventObject(this);
	ProcessWindowEvent(updateEvent);
}

void VideoPlayerFrame::OnToolbarCheck(wxCommandEvent& event) {
	if (event.IsChecked()) {
		splitter->SetSashInvisible(false);
		splitter->SetMinimumPaneSize(cMinPaneSize);
		splitter->SetSashPosition(cSashPosition); // if showing, restore sash position
	} else {
		cMinPaneSize = splitter->GetMinimumPaneSize();
		cSashPosition = splitter->GetSashPosition(); // if hiding, save sash position

		splitter->SetSashInvisible(true);
		splitter->SetMinimumPaneSize(0);
		wxSize size = splitter->GetClientSize(); // Get the client size of the window
		splitter->SetSashPosition(size.GetWidth());
	}

	splitter->UpdateSize();
}
