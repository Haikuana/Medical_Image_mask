#pragma once
#ifndef _DATA_TYPES_
#define _DATA_TYPES_

#include <CGAL/Cartesian.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Bbox_3.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <vector>
#include <list>
#include <set>
#include <string>
#include <assert.h>
#include <QString>
#include <QObject>
#include <fstream>
#include <algorithm>
#include <QDir>
#include <io.h>
//#include <gl/glut.h>

#define IMAGEWIDTHSIZE 0.5

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2                                          Point_2;
typedef K::Point_3                                          Point_3;
typedef K::Vector_2                                         Vector_2;
typedef K::Vector_3                                         Vector_3;

typedef CGAL::Bbox_2 Bbox_2;
typedef CGAL::Bbox_3 Bbox_3;

using namespace std;

enum WindowType
{
	OPERATION_WINDOW, GROUND_WINDOW, EVERY5_WINDOW, ALLMASK_WINDOW, MASKVOXELS_WINDOW
};

struct color
{
	double r;
	double g;
	double b;
};

struct Pixel
{
	int row;
	int col;
	int index;
	int gray_value;
};

#endif