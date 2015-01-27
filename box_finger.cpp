#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include "finger.h"

using namespace cv;
using namespace std;

//プロトタイプ宣言
Point2d find_color_point(Mat src, Color color);
int maxlist(vector<vector<Point> > cnt);
Point2d moment(Mat mask);

int main(int argc, char **argv){
	//初期化
	VideoCapture capl(0);
	VideoCapture capr(0);
	capl.open(0);
	capr.open(1);

	capl.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capl.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	if(!capl.isOpened()) return -1;
	namedWindow("left", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);

	capr.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capr.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	if(!capr.isOpened()) return -1;
	namedWindow("right", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	//-------------ここまで初期化-------------

	//見つけた点を打つつもりのウィンドウ
	//Mat black_img = Mat::zeros(Size(640, 480) , CV_8UC3);
	//namedWindow("black", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);


	//8点探し部分のループ
	Mat frameL, frameR;
	bool colors_flag = false;

	Color color_set[8];
	Color yellow_green; //markar
	Point2d marker_L, marker_R;

	Point2d color_point_L[8];
	Point2d color_point_R[8];

	Point2d facef_L[4], faceb_L[4];
	Point2d facef_R[4], faceb_R[4];
	//set_colors(H_min, H_max, S_min, S_max, V_min, V_max);
	//
	//  /4---------/5
	// / |       / |
	//+0-------1+  |
	//|  |7-----|-6|
	//| /       |  /
	//|/        | /
	//+3-------2+/
	//
	//
	//+0--------------------+1
	//|\                   /|
	//|  \               /  |
	//|   +4------------+5  |
	//|   |             |   |
	//|   |             |   |
	//|   |             |   |
	//|   |             |   |
	//|   |             |   |
	//|   +7------------+6  |
	//|  /               \  |
	//|/                   \|
	//+3--------------------+2
	//
	
	color_set[0].set_colors(251, 7,   56, 96,  0,  82); //red 
	color_set[1].set_colors(3,   25,  48, 100, 53, 100); //orange
	color_set[2].set_colors(25,  72,  45, 100, 0,  100); //yellow 
	color_set[3].set_colors(173, 360, 53, 79,  64, 100); //pink
	color_set[4].set_colors(0,   21,  27, 77,  0,  52); //brown
	color_set[5].set_colors(61,  101, 14, 100, 0,  100); //green
	color_set[6].set_colors(148, 235, 23, 100, 17, 90); //purple
	color_set[7].set_colors(88,  145, 45, 100, 19, 100); //sky

	yellow_green.set_colors(61, 72, 17, 100, 0, 100);
	//cout << color_set[0].H_max << color_set[0].H_min << endl;
	
	capl >> frameL;
	capr >> frameR;

	//ポイントを探す
	for (int i = 0; i < 8; i++){
		color_point_L[i] = find_color_point(frameL, color_set[i]);
		color_point_R[i] = find_color_point(frameR, color_set[i]);
		
		if(i < 4){	
			facef_L[i] = color_point_L[i];
			facef_R[i] = color_point_R[i];
		}else if(i >= 4){	
			faceb_L[i - 4] = color_point_L[i];
			faceb_R[i - 4] = color_point_R[i];
		}

		cout << "L_" << i << ": "<< color_point_L[i] << endl; 
		cout << "R_" << i << ": "<< color_point_R[i] << endl; 
		cout << "diff" << i << ": "<< color_point_L[i].x - color_point_R[i].x << "," << color_point_L[i].y - color_point_R[i].y	<< endl;	
	}
	//マーカーの位置をついでに計測
	//: テスト
	marker_R = find_color_point(frameR, yellow_green);
	marker_L = find_color_point(frameL, yellow_green);
	cout << "markar :" << marker_L.x - marker_R.x << ", " << marker_L.y - marker_R.y << endl;

	cout << "size: (x = " << facef_L[1].x - facef_L[0].x << ", y = " << facef_L[3].y - facef_L[0].y << ")" << endl;
	cout << "size: (x = " << facef_R[1].x - facef_R[0].x << ", y = " << facef_R[3].y - facef_R[0].y << ")" << endl;

	//imshow("frameL", frameL);
	//imshow("frameR", frameR);

	//視差の計算
	//とりあえず正面と背面の視差の平均値を使って、マーカーの距離を相対的に探す
	//  --> x
	// |
	// V y
	//
	Point2d f_diff, b_diff;
	Point2d f_sum_diffs, b_sum_diffs;

	//手法買えるときにでも関数化する
	for(int j = 0; j < 4; j++){
		f_sum_diffs += (facef_L[j] - facef_R[j]);
		b_sum_diffs += (faceb_L[j] - faceb_R[j]);
	}
	
	f_diff.x = f_sum_diffs.x /4.0;
	f_diff.y = f_sum_diffs.y /4.0;
	b_diff.x = b_sum_diffs.x /4.0;
	b_diff.y = b_sum_diffs.y /4.0;

	cout << "fx: "<<f_diff.x << ", fy: " << f_diff.y << endl <<"bx: " << b_diff.x << ", by: " << b_diff.y << endl;

	int box_size = abs((int)f_diff.x);

	//面1の視差を計算して…
	////1. 平均を取る
	//2. 面1の各点毎に差をとって左右の傾きも取る
	//同様に面2も計算
	//
	//比を計算してマーカーが空間のどこにあるか、奥行きを計算
	////所詮大したサイズにならないし視差行列作っちゃう?
	

	//   //|\
	//  /  | \
	//  d_s|  \
	//    \+---\
	//depth|b_x \
	//   \ |     \
	//    \+------\
	//    	front_x
	//d(depth)を求める
	
	double d_s = 0.0 ,depth = 0.0;

	d_s = (f_diff.x*b_diff.x) / (f_diff.x - b_diff.x);
	depth = d_s + f_diff.x;

	cout << "depth : " << depth << endl;

	Point3d marker_xyz;

	marker_xyz.x = marker_L.x - facef_L[0].x;
	marker_xyz.y = marker_L.y - facef_L[0].y;
	marker_xyz.z = (f_diff.x - (marker_L.x - marker_R.x))* depth/ f_diff.x;

	cout << marker_xyz << endl;

	//検知したマーカーの座標入れる配列
	//boxel[x][y][z]
	bool boxel[box_size][box_size][box_size];
	for(int i=0; i < box_size; i++){
		for(int j=0; j < box_size; j++){
			for(int k=0; k < box_size; k++){
				boxel[i][j][k] = 0;
			}
		}
	}
	
	while(1){
		capl >> frameL;
		capr >> frameR;

		imshow("left", frameL);
		imshow("right", frameR);
		
		
		
		if(waitKey(1) == 27){
			break;
		}
	}
	
	return 0;
}
