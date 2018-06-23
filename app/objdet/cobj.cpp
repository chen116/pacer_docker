/* to compile:
g++ cobj.cpp -o app `pkg-config --cflags --libs opencv`
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
    Mat img, gray;
	
    img = imread("image.jpg", IMREAD_COLOR);
    int start_s=clock();

    cvtColor(img, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray,Size(7, 7), 1.5);
    Canny(gray, gray, 0, 50);
	// the code you wish to time goes here
	int stop_s=clock();
	std::cout << "cpu time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;

    imshow("edges", gray);
    waitKey();




    UMat uimg, ugray;

    imread("image.jpg", IMREAD_COLOR).copyTo(uimg);
    int ustart_s=clock();
    cvtColor(uimg, ugray, COLOR_BGR2GRAY);
    GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
    Canny(ugray, ugray, 0, 50);
 	int ustop_s=clock();
	std::cout << "gpu time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;

    imshow("edges", gray);
    waitKey();



}