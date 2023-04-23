#pragma once
#include <IconicMeasureCommon/exports.h>
#include <wx/glcanvas.h>
#include <wx/generic/filectrlg.h>

namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT ColorBox : public wxControl
	{
	public:
		ColorBox(wxWindow* parent, wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxBORDER_NONE);

		void SetColor(const wxColour& color);

	private:
		wxColour m_color;

		void OnPaint(wxPaintEvent& event);

		wxDECLARE_EVENT_TABLE();
	};
}