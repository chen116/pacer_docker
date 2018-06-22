#include "opencv2/opencv.hpp"
#include <ctime>// include this header
#include <iostream>
using namespace cv;
 
int main(int argc, char** argv)
{
    Mat img, gray;
	
    int start_s=clock();
    img = imread("image.jpeg", IMREAD_COLOR);
    cvtColor(img, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray,Size(7, 7), 1.5);
    Canny(gray, gray, 0, 50);
     
    // imshow("edges", gray);
    // waitKey();

	// the code you wish to time goes here
	int stop_s=clock();
	std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;



    UMat uimg, ugray;
    int ustart_s=clock();

    imread("image.jpeg", IMREAD_COLOR).copyTo(uimg);
     
    cvtColor(uimg, ugray, COLOR_BGR2GRAY);
    GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
    Canny(ugray, ugray, 0, 50);
     
    // imshow("edges", gray);
    // waitKey();
	int ustop_s=clock();
	std::cout << "time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;


}