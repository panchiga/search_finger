#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

class Color
{
	public:
		int H_max;
		int H_min;
		int S_max;
		int S_min;
		int V_max;
		int V_min;

		void set_colors(int Hmin, int Hmax, int Smin, int Smax, int Vmin, int Vmax){
			H_min = Hmin;	
			H_max = Hmax;
			S_min = Smin;
			S_max = Smax;
			V_min = Vmin;
			V_max = Vmax;
		}
};

//プロトタイプ宣言
Point2d find_color_point(Mat src, Color color);
int maxlist(vector<vector<Point> > cnt);
Point2d moment(Mat mask);
