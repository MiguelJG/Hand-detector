#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame;
	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la c�mara\n");
		return -1;
	}
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame v�lido\n");
                exit(-1);
        }


	namedWindow("Reconocimiento");
	namedWindow("Fondo");
	moveWindow("Reconocimiento", 20,20);
	moveWindow("Fondo", 20000,20);

	MyBGSubtractorColor BGsubtractor(cap);
	BGsubtractor.LearnModel();

	for (;;)
	{
		cap >> frame;
		//flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Le�do frame vac�o\n");
			continue;
		}
		int c = cvWaitKey(40);
		if ((char)c == 'q') break;
		Mat hls_frame;
		cvtColor(frame,hls_frame,CV_BGR2HLS);
		BGsubtractor.ObtainBGMask(hls_frame, bgmask);
		dilate(bgmask,bgmask,20);
		dilate(bgmask,bgmask,20);
		medianBlur(bgmask,bgmask,3);
		HandGesture hand;
		hand.FeaturesDetection(bgmask, frame);

		imshow("Reconocimiento", frame);
		imshow("Fondo", bgmask);

	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}
