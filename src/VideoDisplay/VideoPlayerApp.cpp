#include <VideoDisplay/VideoPlayerApp.h>
#include <IconicMeasureCommon/MeasureHandler.h>
#include <IconicMeasureCommon/VideoPlayerFrame.h>
#include <IconicGpu/Version.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

wxIMPLEMENT_APP(IconicMeasureApp);

bool IconicMeasureApp::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}

	wxImage::AddHandler(new wxPNGHandler);

	// Version info for about box etc.
	SetAppDisplayName("Iconic Measure");
	SetAppName("Iconic Measure");
	SetVendorName("I-CONIC Vision AB");

	wxString description = _("Simple sample application to do 3D measurements in 2D images, using a precomputed depth map.\nSystem description : ") + wxGetOsDescription();
	cpVersionInfo = boost::shared_ptr<wxVersionInfo>(new wxVersionInfo(_(""), ICONIC_MAJOR_VERSION, ICONIC_MINOR_VERSION, ICONIC_MICRO_VERSION, description, _("(C) 2023 I-CONIC Vision AB")));

	wxFileName f(wxStandardPaths::Get().GetExecutablePath());
	f.AppendDir(wxString("plugins"));
	wxString appPath(f.GetPath());
	GpuVideoDecoder::SetPluginDirectory(appPath.char_str());

	iconic::MeasureHandlerPtr pHandler(new iconic::MeasureHandler);
	cpFrame = new iconic::common::VideoPlayerFrame(GetAppDisplayName(), cpVersionInfo, 0, true, pHandler);
	cpFrame->Show(true);

	return true;
}