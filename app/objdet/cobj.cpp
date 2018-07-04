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

#include <heartbeats/heartbeat.h>

heartbeat_t* heart;





static const int64_t vic_win_size = 10;
static const int64_t vic_buf_depth = 100;
static const char* vic_log_file ="vic.log";
static const int64_t vic_min_target = 100;
static const int64_t vic_max_target = 100;

int main(int argc, char** argv)
{

    int var=33;
    double t_start= 0;
    int start_s=clock();
//normal code
    Mat img, gray;
    img = imread("image.jpg", IMREAD_COLOR);
    // imshow("normal", img);
    // waitKey();

    
heart = heartbeat_init(vic_win_size, vic_buf_depth, vic_log_file, vic_min_target, vic_max_target);


int cnt=0;
while (cnt<3)
{
    cnt++;
        t_start = (double)getTickCount();   
        cvtColor(img, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray,Size(7, 7), 1.5);
        Canny(gray, gray, 0, 50);
    	// the code you wish to time goes here
        int stop_s=clock();
        // std::cout << "cpu time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
        t_start = (double)getTickCount() - t_start;    
        printf( "%d detection time = %g ms\n", 0,t_start*1000/getTickFrequency());
        // imshow("edges", gray);


        // waitKey();
    heartbeat(heart, cnt);





        t_start = (double)getTickCount();

        UMat uimg, ugray;

        imread("image.jpg", IMREAD_COLOR).copyTo(uimg);
        int ustart_s=clock();

        cvtColor(uimg, ugray, COLOR_BGR2GRAY);
        GaussianBlur(ugray, ugray,Size(7, 7), 1.5);
        Canny(ugray, ugray, 0, 50);
     	int ustop_s=clock();
    	// std::cout << "gpu time: " << (ustop_s-ustart_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
        t_start = (double)getTickCount() - t_start;    
        printf( "%d detection time = %g ms\n", 1,t_start*1000/getTickFrequency());
        // imshow("edges", ugray);
        // waitKey();


       printf("heartbeat: windowed rate: %f\n",hb_get_windowed_rate(heart) );
       printf("heartbeat: instant rate: %f\n",hb_get_instant_rate(heart) );

}
    heartbeat_finish(heart);

}