#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include	<boost/shared_ptr.hpp>

namespace iconic {
	namespace common {
		class VideoPlayerFrame; // Forward declaration
	}
}

//! The video player application
/** OnInit is called as an equivalent to main().*/
class IconicMeasureApp : public wxApp {
public:
	//! Called on application startup
	/** Creates the main frame */
	virtual bool OnInit();

private:
	iconic::common::VideoPlayerFrame* cpFrame; // The main frame
	boost::shared_ptr<wxVersionInfo> cpVersionInfo; // Version information for about box
};