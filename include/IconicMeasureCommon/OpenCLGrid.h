#pragma once

#include	<wx/grid.h>
#include	<wx/dialog.h>
#include	<IconicMeasureCommon/exports.h>

namespace iconic {
namespace common {
//! A table with OpenCL capabilities on current platform
/**
\sa OpenCLDialog
*/
class ICONIC_MEASURE_COMMON_EXPORT OpenCLGrid : public wxGrid {
public:
    //! Constructor
    /** Uses boost::compute to retrieve OpenCL platform capabilites*/
    OpenCLGrid(wxWindow *pParent);
};

//! A dialog with OpenCL capabilities on current platform
/**
\sa OpenCLGrid
*/
class ICONIC_MEASURE_COMMON_EXPORT OpenCLDialog : public wxDialog {
public:
    //! Constructor
    /** Creates an OpenCLGrid and shows it with a default OK button in a dialog.*/
    OpenCLDialog(wxWindow *pParent);
};
}
}