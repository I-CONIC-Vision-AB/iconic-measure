#include <IconicMeasureCommon/ColorBox.h>
#include <wx/wx.h>

using namespace iconic;

wxBEGIN_EVENT_TABLE(ColorBox, wxControl)
EVT_PAINT(ColorBox::OnPaint)
wxEND_EVENT_TABLE()


ColorBox::ColorBox(wxWindow* parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size,
	long style)
	: wxControl(parent, id, pos, size, style) {
	m_color = wxColour(0, 0, 0); // initial color is black
}

void ColorBox::SetColor(const wxColour& color) {
	m_color = color;
	Refresh();
}

void ColorBox::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);

	dc.SetBrush(wxBrush(m_color));
	dc.DrawRectangle(GetClientRect());
}
