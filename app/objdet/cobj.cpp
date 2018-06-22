#include "opencv2/opencv.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include <ctime>// include this header
#include <iostream>
using namespace cv;
 
int main(int argc, char** argv)
{
    Mat img, gray;
	
    int start_s=clock();
    img = cv::imread("image.jpeg", IMREAD_COLOR);
    cv::cvtColor(img, gray, COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray,Size(7, 7), 1.5);
    cv::Canny(gray, gray, 0, 50);
     
    // imshow("edges", gray);
    // waitKey();

	// the code you wish to time goes here
	int stop_s=clock();
	std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;



    UMat uimg, ugray;
    int ustart_s=clock();

    cv::imread("image.jpeg", IMREAD_COLOR).copyTo(uimg);
     
    cv::cvtColor(uimg, ugray, COLOR_BGR2GRAY);
    cv::GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
    cv::Canny(ugray, ugray, 0, 50);
     
    // imshow("edges", gray);
    // waitKey();
	int ustop_s=clock();
	std::cout << "time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;


}