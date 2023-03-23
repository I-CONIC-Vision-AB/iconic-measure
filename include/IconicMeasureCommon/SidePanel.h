
#include <wx/wx.h>
#include <IconicMeasureCommon/exports.h>


namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT SidePanel : public wxPanel {

	public:
		SidePanel(wxWindow* parent);
		//not how this will be done later
		void SetAreaText(wxString text);

	private:
		wxBoxSizer* sizer;
		wxStaticText* area_text;
		wxStaticText* area_value;
	};
}