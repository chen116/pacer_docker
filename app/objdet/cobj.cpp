/* to compile:
g++ cobj.cpp -o app `pkg-config --cflags --libs opencv` -std=c++11 
*/
#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;




int main(int argc, char** argv)
{


//normal code
    Mat img, gray;
    double t_start= 0;
    int start_s=clock();
    t_start = (double)getTickCount();	
    img = imread("image.jpg", IMREAD_COLOR);


    cvtColor(img, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray,Size(7, 7), 1.5);
    Canny(gray, gray, 0, 50);
	// the code you wish to time goes here
    int stop_s=clock();
    std::cout << "cpu time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
    t_start = (double)getTickCount() - t_start;    
    printf( "%d detection time = %g ms\n", 0,t_start*1000/getTickFrequency());
    imshow("edges", gray);


    waitKey();




    UMat uimg, ugray;

    imread("image.jpg", IMREAD_COLOR).copyTo(uimg);
    int ustart_s=clock();
    t_start = (double)getTickCount();

    cvtColor(uimg, ugray, COLOR_BGR2GRAY);
    GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
    Canny(ugray, ugray, 0, 50);
 	int ustop_s=clock();
	std::cout << "gpu time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
    t_start = (double)getTickCount() - t_start;    
    printf( "%d detection time = %g ms\n", 1,t_start*1000/getTickFrequency());
    imshow("edges", ugray);
    waitKey();



}