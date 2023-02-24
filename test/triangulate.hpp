#pragma once

#include <IconicGpu/Triangulator.h>
#include <wx/intl.h>
#include <wx/dcsvg.h>
#include <wx/gdicmn.h>
#include <ctime>
#include <chrono>

// Helper function to write triangulation to SVG file
void saveAsSvg(wxString const& svgFileName, std::vector<Eigen::Vector2f>& vPoints, std::vector<Eigen::Vector3i>& triangles, int width, int height, bool bShow = false, bool bPrintToConsole = false, bool bDrawText = false)
{
	// Write triangles to SVG file
	int nPoints = vPoints.size();
	int svgOffset[2] = { 20, 20 };

	wxSVGFileDC* svg = new wxSVGFileDC(svgFileName, width + svgOffset[0], height + svgOffset[1]);
	svg->DrawText(wxString::Format("Delaunay triangulation of %d random 2D points", nPoints), svgOffset[0], 0);
	wxAffineMatrix2D translate;
	translate.Translate(svgOffset[0], svgOffset[1]);
	BOOST_TEST(svg->SetTransformMatrix(translate) == false); // If this ever returns true we can skip the offsets in the code below!

	// Note: We draw all lines in each triangle, so each line is drawn twice...
	BOOST_TEST(triangles.size() > 0);
	Eigen::Vector2f* pt[3], invalidPt(-1.0f, -1.0f);
	wxPoint r0, r1;
	svg->SetPen(wxPen(wxColour(0, 0, 255)));
	for (int i = 0; i < triangles.size(); ++i) {
		// Indexes to points for triangle i
		Eigen::Vector3i const& ptIdx = triangles[i];

		for (int j = 0; j < 3; ++j) {
			if (ptIdx[j] >= 0 && ptIdx[j] < vPoints.size()) {
				// Let pt point at the valid point
				pt[j] = &(vPoints[ptIdx[j]]);
			}
			else {
				// Invalid point (e.g. index to a fictive point that surrounds the triangulated area)
				pt[j] = &invalidPt;
			}
		}
		for (int j = 0; j < 3; ++j) {
			int j1 = (j + 1) % 3; // Wrap around 3 -> 0
			if (pt[j] != &invalidPt && pt[j1] != &invalidPt) {
				// Both points valid. Draw a line between points
				r0.x = (*pt[j])[0] + svgOffset[0];
				r0.y = height - (*pt[j])[1] + svgOffset[1];
				r1.x = (*pt[j1])[0] + svgOffset[0];
				r1.y = height - (*pt[j1])[1] + svgOffset[1];
				svg->DrawLine(r0, r1); // Inefficient, should use DrawLines and add all lines once. Doesn�t matter for this test though
			}
		}
		if (bPrintToConsole) {
			wxLogVerbose("Triangle %d: [%d %d %d] (%.0f %.0f) (%.0f %.0f) (%.0f %.0f)", i, ptIdx[0], ptIdx[1], ptIdx[2], (*pt[0])[0], (*pt[0])[1], (*pt[1])[0], (*pt[1])[1], (*pt[2])[0], (*pt[2])[1]);
		}
	}

	// Draw the points
	svg->SetPen(wxPen(wxColour(255, 0, 0), 1));
	svg->SetTextBackground(wxColour(255, 255, 255));
	svg->SetBackgroundMode(wxBRUSHSTYLE_SOLID);
	wxFont font = svg->GetFont();
	font.SetPointSize(8);
	svg->SetFont(font);
	for (int i = 0; i < nPoints; ++i) {
		float x = vPoints[i][0] + svgOffset[0];
		float y = height - vPoints[i][1] + svgOffset[1];

		if (bDrawText) {
			// draw text
			auto text = std::to_string(i);
			auto extent = svg->GetTextExtent(text);
			svg->DrawText(text, wxPoint{ (int)(x - extent.x / 2), (int)(y - extent.y / 2) });
		}
		else {
			// draw a circle
			svg->DrawCircle(x, y, 1);
		}
	}

	delete svg;

	if (bShow) {
		wxLaunchDefaultApplication(svgFileName);
	}
}

BOOST_AUTO_TEST_CASE(triangulator_test)
{
	std::cerr << "\nRunning test case: " << boost::unit_test::framework::current_test_case().p_name << std::endl;
	const wxString sOutputFile("C:/Data/TIN.svg"); // Hard coded output path, change if needed
	bool bShowResult = true; // Open default app for SVG files
	bool bPrintTrianglesToConsole = false; // Print triangles and coordinates to console
	bool bDrawText = false; // set to true to show point index instead of circles

	wxLog::SetActiveTarget(new wxLogStderr); // Log to console
	wxLog::SetVerbose(true); // Log wxLogVerbose in additions to errors, warning, messages

	// Generate some random point in a 2D grid (width x height)
	size_t seed = time(NULL), nPoints = 1000;
	std::vector<Eigen::Vector2f> vPoints(nPoints);
	int width(1024), height(1024);
	std::cout << "triangulating " << nPoints << " using seed " << seed << std::endl;
	std::srand(seed);
	float rnd;
	for (int i = 0; i < nPoints; ++i) {
		rnd = (float)std::rand();
		vPoints[i][0] = (rnd / RAND_MAX) * width;
		rnd = (float)std::rand();
		vPoints[i][1] = (rnd / RAND_MAX) * height;
	}

	// Create a 2D Delaunay triangulator
	boost::shared_ptr<iconic::gpu::Triangulator> pTri(iconic::gpu::Triangulator::Get(iconic::gpu::Triangulator::DELAUNAY_2D));
	BOOST_REQUIRE(pTri != nullptr);

	// Generate TIN
	auto start_time = std::chrono::high_resolution_clock::now();
	auto triangulated = pTri->Triangulate(vPoints, 0, nPoints - 1);
	BOOST_TEST(triangulated);
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds duration = end_time - start_time;
	std::cout << "wall time: " << duration.count() / 1e6 << "ms" << std::endl;

	if (triangulated && nPoints <= 10000) {
		auto triangles = pTri->GetTriangles();
		saveAsSvg(sOutputFile, vPoints, triangles, width, height, bShowResult, bPrintTrianglesToConsole, bDrawText);
	}

	delete 	wxLog::SetActiveTarget(nullptr); // Explicitly delete the active target (needed in tests)
}

