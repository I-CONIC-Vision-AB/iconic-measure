#include	<IconicMeasureCommon/OpenCLGrid.h>
#include	<boost/compute.hpp>
#include	<boost/foreach.hpp>
#include	<wx/sizer.h>

using namespace iconic::common;

OpenCLGrid::OpenCLGrid(wxWindow *pParent) :
    wxGrid(pParent, wxID_ANY)
{
    wxString sPlatforms;

    std::vector<boost::compute::platform> platforms = boost::compute::system::platforms();

    CreateGrid(platforms.size(), 7);
    HideRowLabels();
    SetColLabelValue(0, _("Name"));
    SetColLabelValue(1, _("Version"));
    SetColLabelValue(2, _("Profile"));
    SetColLabelValue(3, _("Device name"));
    SetColLabelValue(4, _("Driver version"));
    SetColLabelValue(5, _("OpenCL version"));
    SetColLabelValue(6, _("Type"));

    std::string type;
    for (size_t j=0; j<platforms.size(); ++j) {
        boost::compute::platform &p = platforms[j];
        this->SetCellValue(j, 0, p.name());
        this->SetCellValue(j, 1, p.version());
        this->SetCellValue(j, 2, p.profile());

        std::vector<boost::compute::device> devices = p.devices();
        size_t n = devices.size();
        for (size_t i = 0; i < n; i++) {
            boost::compute::device const &d = devices[i];
            switch (d.type()) {
            case boost::compute::device::cpu:
                type = "CPU";
                break;

            case boost::compute::device::gpu:
                type = "GPU";
                break;

            case boost::compute::device::accelerator:
                type = "ACCELERATOR";
                break;

            default:
                type = "Unknown device type";
            }

            this->SetCellValue(j, 3, d.name());
            this->SetCellValue(j, 4, d.driver_version());
            this->SetCellValue(j, 5, d.version());
            this->SetCellValue(j, 6, type);
        }
    }
}

OpenCLDialog::OpenCLDialog(wxWindow *pParent) :
    wxDialog(pParent, wxID_ANY, _("OpenCL capabilities"))
{
    wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);
    OpenCLGrid *pGrid = new OpenCLGrid(this);
    pSizer->Add(pGrid, 1, wxALL | wxEXPAND, 2);
    pSizer->Add(this->CreateButtonSizer(wxOK), 1, wxALL | wxEXPAND, 0);
    pGrid->Fit();
    SetSizerAndFit(pSizer);
}
