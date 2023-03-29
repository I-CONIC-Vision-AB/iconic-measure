#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include	<IconicMeasureCommon/Defines.h>
#include	<IconicMeasureCommon/MeasureEvent.h>
#include	<IconicVideo/GpuVideoDecoder.h>
#include	<IconicVideo/PluginLoader.h>
#include	<IconicMeasureCommon/Defines.h>
#include	<IconicMeasureCommon/MeasureHandler.h>
#include	<IconicMeasureCommon/ImageCanvas.h>
#include    <IconicGpu/OutputStream.h>
#include	<boost/timer/timer.hpp>
#include    <wx/filename.h>
#include    <wx/datetime.h>

namespace iconic {
	namespace common {


		/**
		* @brief The main frame of the application.
		*
		* We will show the video window as a direct child to this frame.
		* @todo Create a toolbar with options for selecting geometry primitive, e.g. polygon, vectors, points
		*/
		class ICONIC_MEASURE_COMMON_EXPORT VideoPlayerFrame : public wxFrame {
		public:


			//! Constructor
			VideoPlayerFrame(wxString const& title, boost::shared_ptr<wxVersionInfo> pVersionInfo, int streamNumber = 0, bool bImmediateRefresh = true, iconic::MeasureHandlerPtr pHandler = iconic::MeasureHandlerPtr());

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
			 * @brief Step one frame/image ahead
			 * @param WXUNUSED
			*/
			void OnNextImage(wxCommandEvent& WXUNUSED(event));

			/**
			 * @brief Save decoded images
			*/
			void OnSave(wxCommandEvent& WXUNUSED(e));

			//! Quit application
			void OnQuit(wxCommandEvent& event);

			/**
			 * @brief Show about box
			 * @param event Unused
			 * @todo Add your names to "Developers"
			*/
			void OnAbout(wxCommandEvent& event);

			//! Toggle fullscreen mode
			void OnFullscreen(wxCommandEvent& e);

			//! Pause/play video
			void OnPause(wxCommandEvent& e);

			//! Keep asking for more frames as long as there are any.
			/** Calls HandleIdle which handles the last decoded video frame.
			\sa HandleIdle*/
			void OnIdle(wxIdleEvent& e);

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

			/**
			 * @brief Set mouse mode.
			 *
			 * Toggle between moving (and zooming) or measuring
			 * @param mode Mouse mode
			 * @todo Set mouse mode of the image window accordingly
			*/
			void SetMouseMode(ImageCanvas::EMouseMode mode);

			/**
			 * @brief Get mouse mode.
			 * @return Mouse mode
			 * @sa SetMouseMode
			*/
			ImageCanvas::EMouseMode GetMouseMode() const;

			/**
			 * @brief Updates current action (e.g. view, measure) based on active toolbar button.
			 * 
			*/
			void OnToolbarPress(wxCommandEvent& e);

		protected:

			/**
			 * @brief Creates the menu for the main frame
			 * @todo Create options for selecting geometry primitive, e.g. polygon, vectors, points
			*/
			void CreateMenu();

			//! Called by timer if normal speed is selected
			void OnTimer(wxTimerEvent& WXUNUSED(e));

			//! Toggle playing video at normal or maximum speed
			void OnUseTimer(wxCommandEvent& e);

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

			/**
			 * @brief Toggle mouse mode
			 * @param WXUNUSED
			*/
			void OnMouseMode(wxCommandEvent& WXUNUSED(e));

			/**
			 * @brief Handles a measured point.
			 *
			 * The event is sent from ImageCanvas when in measure mode and mouse left clicked.
			 * @param e Contains point and mouse action
			 * @todo Handle the measured point, e.g. by adding it to a polygon
			*/
			void OnMeasuredPoint(MeasureEvent& e);

			/**
			 * @brief Checks/unchecks measure menu item
			 * @param e check/uncheck
			*/
			void OnMouseModeUpdate(wxUpdateUIEvent& e);

			//! Create video decoder
			bool CreateDecoder();

			//! \cond
			iconic::ImageCanvas* cpImageCanvas; // The video window
			VideoDecoderPtr cpDecoder; // The video decoder
			boost::shared_ptr<wxVersionInfo> cpVersionInfo; // Version information for about box
			int cStreamNumber;
			bool cbRefresh;
			bool cbPause;

			wxTimer cTimer;
			bool cbUseTimer;
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
			wxString cPath;

			iconic::MeasureHandlerPtr cpHandler;


			wxDECLARE_EVENT_TABLE();

			wxToolBar* toolbar;
		};
	}
}