
#include <wx/wx.h>
#include <IconicMeasureCommon/exports.h>
#include <boost/shared_ptr.hpp>
#include <IconicMeasureCommon/Geometry.h>
#include <vector>


namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT SidePanel : public wxPanel {

	public:
		SidePanel(wxWindow* parent);
		void Update(std::vector <boost::shared_ptr<iconic::Geometry::Shape>> shapes);

	private:
		wxBoxSizer* sizer;
	};
}