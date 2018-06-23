#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace std;
using namespace cv;
int main(int argc, char** argv)
{
    Mat img, gray;
	
    int start_s=clock();
    img = imread("cat.jpg", IMREAD_COLOR);
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

    imread("cat.jpg", IMREAD_COLOR).copyTo(uimg);
     
    cvtColor(uimg, ugray, COLOR_BGR2GRAY);
    GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
    Canny(ugray, ugray, 0, 50);
     
    // imshow("edges", gray);
    // waitKey();
	int ustop_s=clock();
	std::cout << "time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;


}