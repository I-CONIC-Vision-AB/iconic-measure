#include	<VideoDisplay/VideoPlayerApp.h>
#include	<IconicMeasureCommon/IconicMeasureHandler.h>
#include	<IconicMeasureCommon/VideoPlayerFrame.h>
#include	<IconicGpu/Version.h>
#include    <wx/filename.h>
#include    <wx/stdpaths.h>

wxIMPLEMENT_APP(IconicMeasureApp);

bool IconicMeasureApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    // Version info for about box etc.
    SetAppDisplayName("Iconic Measure");
    SetAppName("Iconic Measure");
    SetVendorName("I-CONIC Vision AB");

    wxString description = _("Simple sample application to display videos. Decoding and rendering occur on GPU.\nSystem description : ") + wxGetOsDescription();
    cpVersionInfo = boost::shared_ptr<wxVersionInfo>(new wxVersionInfo(_(""), ICONIC_MAJOR_VERSION, ICONIC_MINOR_VERSION, ICONIC_MICRO_VERSION, description, _("(C) 2022 I-CONIC Vision AB")));

    wxFileName f(wxStandardPaths::Get().GetExecutablePath());
    f.AppendDir(wxString("plugins"));
    wxString appPath(f.GetPath());
    GpuVideoDecoder::SetPluginDirectory(appPath.char_str());

    IconicMeasureHandlerPtr pHandler(new IconicMeasureHandler);
    cpFrame = new iconic::common::VideoPlayerFrame(GetAppDisplayName(), cpVersionInfo);
    cpFrame->Show(true);

    return true;
}