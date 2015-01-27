#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

Point2i test(Mat src, int H, int S, int V){
	Point2i pnt(2,3);

	return pnt;
}

struct Color
{
	int h_max;
	int h_min;
};

int main(int argc, char **argv){
	Mat src;
	src = imread(argv[1]);
	Point2i point;

	point = test(src, 100,100,100);
	cout << point.x << point.y; 

	Color hoge;

	hoge.h_max = 10;

	cout << hoge.h_max << endl;

	return 0;
}
