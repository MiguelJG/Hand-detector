#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

HandGesture::HandGesture() {
}


double HandGesture::getAngle(Point s, Point e, Point f) {
	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;
	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;
	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);
	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {
	vector<vector<Point> > contours;
	vector<Point> contoursmax;
	Mat temp_mask ;
	mask.copyTo(temp_mask);//copia la mascara
	int index = -1;
        findContours(temp_mask,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
				for(int aux = 0; aux <contours.size(); aux++){
					if(contours[aux].size() > contoursmax.size()){
						contoursmax = contours[aux];
						index = aux;
					}
				}
				drawContours(output_img,contours,index,Scalar(255,0,0),2,8,vector<Vec4i>(),0,Point());
	vector<int> hull;
	convexHull(contours[index],hull);
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);
		int cont = 0;
		int numcir = 0;
		//double area = contourArea(contours,false);
		double area = contourArea(contoursmax,false);
		std::ostringstream strs;
		strs << area;
		strs << string(" es el area que ocupa la mano");
		std::string str = strs.str();
		string Result;          // string which will contain the result

			if(area > 100000){
				cv::putText(output_img,string("Aleja la mano"),Point(10, output_img.rows / 1.3),FONT_HERSHEY_SIMPLEX,2,CV_RGB(255, 0, 0), 2);
			}
			else if( area < 20000){
				cv::putText(output_img,string("Acerca la mano"),Point(10, output_img.rows / 1.3),FONT_HERSHEY_SIMPLEX,2,CV_RGB(255, 0, 0), 2);
			}
			else{
				for (int i = 0; i < defects.size(); i++) {
					Point s = contours[index][defects[i][0]];
					Point e = contours[index][defects[i][1]];
					Point f = contours[index][defects[i][2]];
					float depth = (float)defects[i][3] / 256.0;//se divide la distancia entre 256 para que este entre 0 255
					double angle = getAngle(s, e, f);
					if(depth > 50 && angle < 90){
						circle(output_img,f,5,Scalar(0,255,0),3);
						numcir++;
					}
		    }
				switch (numcir) {
					case 0:
					 Result = "Tiene 1 o 0 dedos levantados.";
					break;
					case 1:
					Result = "Tiene 2 dedos levantados.";
					break;
					case 2:
					Result = "Tiene 3 dedos levantados.";
					break;
					case 3:
					Result = "Tiene 4 dedos levantados.";
					break;
					case 4:
					Result = "Tiene 5 dedos levantados.";
					break;
					default:
					Result = "No reconocible";
				}
				cv::putText(output_img, //target image
							Result, //text
							Point(10, output_img.rows / 1.2), //top-left position
							FONT_HERSHEY_SIMPLEX,
							1.0,
							CV_RGB(0, 255, 4), //font color
							2);
				cv::putText(output_img, //target image
							str, //text
							Point(10, output_img.rows / 5),
							FONT_HERSHEY_SIMPLEX,
							0.5,
							CV_RGB(0, 0, 0), //font color
							2);
					rectangle(output_img, boundingRect(contoursmax).tl(), boundingRect(contoursmax).br(), CV_RGB(0, 85, 255), 2, 8, 0);
			}

}
