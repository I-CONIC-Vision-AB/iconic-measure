#pragma once

#include <IconicMeasureCommon/Geometry.h>
#include <IconicGpu/Types.h>
#include <vector>

BOOST_AUTO_TEST_CASE(iconic_polygon_test)
{
	std::cerr << "\nRunning test case: " << boost::unit_test::framework::current_test_case().p_name << std::endl;
	const wxString sOutputFile("C:/Data/TIN.svg"); // Hard coded output path, change if needed
	bool bShowResult = true; // Open default app for SVG files
	bool bPrintTrianglesToConsole = true; // Print triangles and coordinates to console
	bool bDrawText = false; // set to true to show point index instead of circles

	wxLog::SetActiveTarget(new wxLogStderr); // Log to console
	wxLog::SetVerbose(true); // Log wxLogVerbose in additions to errors, warning, messages

	std::vector<Eigen::Vector2d> vPolygon = { {0,0}, {0,1} , {1,0} , {0,0} }; // a simple triangle 
	double area = iconic::gpu::Geometry::Area(vPolygon);
	BOOST_CHECK_EQUAL(area, 0.5); // Area of right-angled triangle with sides 1 (1*1)/2 = 0.5

	Eigen::Vector2d ptInside(0.25, 0.25);
	BOOST_TEST(iconic::gpu::Geometry::PointInPolygon(vPolygon, ptInside) == true);

	Eigen::Vector2d ptOutside(1.25, 1.25);
	BOOST_TEST(iconic::gpu::Geometry::PointInPolygon(vPolygon, ptOutside) == false);

	vPolygon.insert(vPolygon.begin()+2, ptInside); // insert the "inside" point so that polygon is { {0,0}, {0,1} , {0.25, 0.25} , {1,0} , {0,0} } Concave+convex
	area = iconic::gpu::Geometry::Area(vPolygon);
	BOOST_CHECK_EQUAL(area, 0.25); // Area of new smaller polygon (the area excluded by adding the new point is correctly excluded from the area and not counted twice)

	delete wxLog::SetActiveTarget(nullptr);
}