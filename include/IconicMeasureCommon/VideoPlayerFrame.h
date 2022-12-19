#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include	<IconicMeasureCommon/Defines.h>
#include	<IconicVideo/GpuVideoDecoder.h>
#include	<IconicVideo/PluginLoader.h>
#include    <IconicGui/ImageCanvasHw.h>
#include	<IconicMeasureCommon/Defines.h>
#include	<IconicMeasureCommon/IconicMeasureHandler.h>
#include    <IconicGpu/OutputStream.h>
#include	<boost/timer/timer.hpp>
#include    <wx/filename.h>
#include    <wx/datetime.h>

// Constants for saving configurations used in dialogs
#define ICONIC_EXPORT_FTP_ADDRESS  "export/ftp/address"
#define ICONIC_EXPORT_FTP_USER  "export/ftp/user"

namespace iconic {
namespace common {

//! The main frame of the application.
/** We will show the video window as a direct child to this frame.*/
class ICONIC_MEASURE_COMMON_EXPORT VideoPlayerFrame : public wxFrame {
public:

    //! Constructor
    VideoPlayerFrame(wxString const &title, boost::shared_ptr<wxVersionInfo> pVersionInfo, int streamNumber = 0, bool bImmediateRefresh = true, IconicMeasureHandlerPtr pHandler = IconicMeasureHandlerPtr());

    //! Destructor
    virtual ~VideoPlayerFrame();

    //! Open video file
    void OnOpen(wxCommandEvent& event);

    /**
     * @brief Open folder with still images
     * @param WXUNUSED
    */
    void OnOpenFolder(wxCommandEvent& WXUNUSED(event));

    /**
     * @brief Save decoded images
    */
    void OnSave(wxCommandEvent& WXUNUSED(e));

    //! Quit application
    void OnQuit(wxCommandEvent& event);

    //! Show about box
    /** Uses version info from constructor. */
    void OnAbout(wxCommandEvent& event);

    //! Toggle fullscreen mode
    void OnFullscreen(wxCommandEvent &e);

    //! Pause/play video
    void OnPause(wxCommandEvent &e);

    //! Keep asking for more frames as long as there are any.
    /** Calls HandleIdle which handles the last decoded video frame.
    \sa HandleIdle*/
    void OnIdle(wxIdleEvent &e);

    //! Shows a table with OpenCL capabilities on this platform.
    /**
    \sa OpenCLDialog
    */
    void OnOpenCLCapabilities(wxCommandEvent& WXUNUSED(event));

    //! Handling last decoded video frame.
    /** Called from OnIdle and OnTimer (event handlers should not be overloaded) .
    	This displays the video frame either at nominal frame rate or maximum speed.
    	ProcessDecodedFrame is called within this method if a frame was retrieved.
    	\sa OnIdle OnTimer ProcessDecodedFrame
    */
    virtual void GetDecodedFrame();

    //! Set video frame rate
    /**
    Set number of frames per second for video playback. This is read from most file types, but you may want a different frame rate.
    */
    void OnSetFrameRate(wxCommandEvent& WXUNUSED(e));

    //! Handles a frame when it has been decoded.
    /**
    	GetDecodedFrame from HandleIdle if a frame has been decoded. This implementation is empty (!), but you can overload this method to e.g. filter the decoded frame.
    \sa HandleIdle
    */
    virtual void ProcessDecodedFrame();

    //! Returns the stream number
    /**
    	The stream number is the index of this video in the singleton GpuProcessor. You get the current GPU image etc for this video through GpuProcessor::GetStream(streamNumber)
    */
    int GetStreamNumber() const;

    //! Use mipmap for better quality
    void SetMipMap(bool set = true);

    //! Use linear interpolation for better quality
    void SetInterpolation(bool set = true);

    //! Load and display a video file.
    virtual void OpenVideo(wxString filename);

    //! Returns name of video if loaded
    wxString GetVideoFileName() const;

protected:
    //! Creates the menu for the main frame
    void CreateMenu();

    //! Called by timer if normal speed is selected
    void OnTimer(wxTimerEvent& WXUNUSED(e));

    //! Toggle playing video at normal or maximum speed
    void OnUseTimer(wxCommandEvent &e);

    void OnUpdatePause(wxUpdateUIEvent& e);
    void OnUpdateFullscreen(wxUpdateUIEvent& e);
    void OnUpdateUseTimer(wxUpdateUIEvent& e);

    //! Starts the timer for frame display
    /** Tries to determine frames per seconds and set to 30 fps if failed.*/
    void StartTimer();

    //! Toggle log in window or message box
    void OnShowLog(wxCommandEvent& e);

    //! Select video decoder to use
    void OnVideoDecoder(wxCommandEvent& WXUNUSED(event));

    //! Only allow selecting video decoder if video has not been opened
    void OnUpdateVideoDecoder(wxUpdateUIEvent& e);

    //! Create video decoder
    bool CreateDecoder();

    /**
     * @brief Create FTP connection etc for export of images.
    */
    bool SetupFtpExport();

    //! \cond
    iconic::ImageCanvasHw *cpImageCanvas; // The video window
    VideoDecoderPtr cpDecoder; // The video decoder
    boost::shared_ptr<wxVersionInfo> cpVersionInfo; // Version information for about box
    int cStreamNumber;
    bool cbRefresh;
    bool cbPause;

    wxTimer cTimer;
    bool cbUseTimer;
    unsigned int cErrorThreshold;
    double cFrameRate, cOriginalFrameRate;
    wxString cFileName;
    bool cbLoop; // Experimental; set to false for now
    wxTextCtrl* cpLogTextCtrl;
    wxLog* cpWindowLog;
    wxLog* cpDefaultLog;
    bool cbIsOpened;
    bool cbFastForward;
    EProtocol cProtocol;

    boost::timer::cpu_timer cClockTimer;

    wxString csVideoDecoderName;
    boost::shared_ptr<iconic::gpu::OutputStream> cpOutputStream1;

    //boost::shared_ptr<wxSocketBase> cpSocket;
    //boost::shared_ptr<wxOutputStream> cpOutputStream;

    iconic::gpu::OutputStream::EOutputType cOutputType;
    wxFileName cOutputFileName;
    int cQuality;
    wxString cPath;
//    unsigned short cuPort;
    boost::shared_ptr<boost::thread_group> cpThreads;

    IconicMeasureHandlerPtr cpHandler;

    wxDECLARE_EVENT_TABLE();
    //! \endcond

};
}
}