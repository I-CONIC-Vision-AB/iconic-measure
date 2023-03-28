
#include <wx/wx.h>
#include <IconicMeasureCommon/exports.h>
#include <boost/shared_ptr.hpp>
#include <IconicMeasureCommon/Geometry.h>
#include <vector>


namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT SidePanel : public wxPanel {

	public:
		SidePanel(wxWindow* parent);
		//not how this will be done later
		void SetShapesPtr(std::vector <boost::shared_ptr<iconic::Geometry::Shape>>* shapes);
		void Update();

	private:
		wxBoxSizer* sizer;
		std::vector <boost::shared_ptr<iconic::Geometry::Shape>>* shapes;
		std::vector <wxPanel*> panels;
	};
}