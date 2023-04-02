#pragma once

#include "exports.h"

//! I-CONIC namespace
namespace iconic {
	//! Common classes for sample programs
	namespace common {
		//! Enumeration of GUI ID:s for sample programs
		enum ESampleGuiId {
			ID_FULLSCREEN = 4000,		//!< Toggle full screen on/off
			ID_PAUSE,					//!< Pause/play video
			ID_NEXT,                    //!< Step one frame/image ahead
			ID_OPENCL_CAPS,				//!< Display OpenCL capabilities
			ID_GRADIENT_PLUS,			//!< Increase gradient threshold
			ID_GRADIENT_MINUS,			//!< Decrease gradient threshold
			ID_GRADIENT_ONLY,			//!< Show only gradient or image as backdrop
			ID_CONTRAST_MIN_PLUS,		//!< Contrast min value
			ID_CONTRAST_MIN_MINUS,		//!< Contrast min value
			ID_CONTRAST_MAX_PLUS,		//!< Contrast max value
			ID_CONTRAST_MAX_MINUS,		//!< Contrast max value
			ID_VIDEO_USE_TIMER,			//!< Toggle use of timer
			ID_VIDEO_TIMER,				//!< Timer for correct frame rate
			ID_VIDEO_SET_FPS,			//!< Set frames per second, frame rate
			ID_VIDEO_SHOW_LOG,			//!< Show log window
			ID_VIDEO_DECODER,			//!< Which decoder to use
			ID_OPEN_FOLDER,				//!< Opena folder with still images
			ID_SHOW_COMPOUND,
			ID_SHOW_COMPOUND2,
			ID_POINT_SIZE,
			ID_PYRAMID_LEVEL,
			ID_EDGE_THRESHOLD,
			ID_MOUSE_MODE,				//!< Toggle mouse behaviour
			ID_TOOLBAR_MOVE,
			ID_TOOLBAR_LINE,
			ID_TOOLBAR_POLYGON,
			ID_TOOLBAR_POINT,
			ID_TOOLBAR_DELETE
		};
	}
}