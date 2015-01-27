#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include "finger.h"

using namespace cv;
using namespace std;

Point2d find_color_point(Mat src, Color color_set){
	Point2d pnt;
	
	Mat color_img;
	int lower[3];
	int upper[3];

	Mat lut = Mat(256, 1, CV_8UC3);

	cvtColor(src, color_img, CV_BGR2HSV);
	double h_opt = 255.0/360;
	double s_opt = 255.0/100;
	double v_opt = 255.0/100;
			 
	lower[0] = int (color_set.H_min * h_opt);
	lower[1] = int (color_set.S_min * s_opt);
	lower[2] = int (color_set.V_min * v_opt);
	
	upper[0] = int (color_set.H_max * h_opt);
	upper[1] = int (color_set.S_max * s_opt);
	upper[2] = int (color_set.V_max * v_opt);

	//lut = look up table
	//これを使って二値化する
	for (int i = 0; i < 255; i++){
		for(int k = 0; k < 3; k++){
			if (lower[k] <= upper[k]){
				if ((lower[k] <= i) && (i <= upper[k])){
					lut.data[i*lut.step+k] = 255;
				}else{
					lut.data[i*lut.step+k] = 0;
				}
			}else{
				if((lower[k] <= i) || (i <= upper[k])){
					lut.data[i*lut.step+k] = 255;
				}else{
					lut.data[i*lut.step+k] = 0;
				}
			}
		}
	}

	//LUTを使って二値化
	LUT(color_img, lut, color_img,0);

	vector<Mat> planes;
	split(color_img, planes);

	Mat mask_img;
	bitwise_and(planes[0], planes[1], mask_img);
	bitwise_and(mask_img, planes[2], mask_img);

	Mat maskedImage;
	//いめーじ.copyTo(コピー先の行列、 マスク)
	//マスクの1の部分だけいめーじのからコピーされる
	
	src.copyTo(maskedImage, mask_img);

	pnt = moment(maskedImage);

	return pnt;
}

int maxlist(vector<vector<Point> > cnt){
	int max_num = 0;
	int max_i = 0;
	
	int size = cnt.size();
	if (size == 0)
		return -1;
	
	for(int i = 0;i < size; i++){
		double cnt_num = cnt[i].size();
		if(cnt_num > max_num){
		 max_num = cnt_num;
		 max_i = i;
		}
	}
	return max_i;
}

Point2d moment(Mat mask){
	//int points[3];
	Point2d xy_point;

	Mat gray_mask;
	Mat hsv_channels[3];
	Mat th;
	split(mask, hsv_channels);
	//hsvの[2]はグレースケールらしい
	gray_mask = hsv_channels[2];
	//とりあえず真ん中で
	threshold(gray_mask, th, 127, 255, 0);
	
	vector<vector<Point> > cnt;
	vector<Vec4i> hiee;
	findContours(gray_mask, cnt, hiee, 1, 2);

	//一番でかい領域の選択
	int max_cnt = maxlist(cnt);
	if (max_cnt != -1){
		Moments m = moments(cnt[max_cnt]);
		drawContours(mask, cnt, max_cnt, Scalar(0,255,255), 2, 8, hiee, 0, Point());
	
		//重心の座標
		int cx,cy;
		//座標がうまく書けない問題
		cx = (int)(m.m10/m.m00);
		cy = (int)(m.m01/m.m00);

		//cout << "moments (" << cx << ", " << cy << ")" << endl;

		xy_point = Point2d(cx, cy);
	}
	return xy_point;
}


