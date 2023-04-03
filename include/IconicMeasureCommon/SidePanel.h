#pragma once
#include <wx/wx.h>
#include <IconicMeasureCommon/exports.h>
#include <boost/shared_ptr.hpp>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/Shape.h>
#include <vector>


namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT SidePanel : public wxScrolled<wxPanel> {

	public:
		SidePanel(wxWindow* parent);
		void Update(std::vector <boost::shared_ptr<iconic::Shape>> shapes);
		void CreatePanel(Shape& shape);

	private:
		void CreatePointPanel(Shape& shape);
		void CreateLinePanel(Shape& shape);
		void CreatePolygonPanel(Shape& shape);

		wxBoxSizer* sizer;
	};
}