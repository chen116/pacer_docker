#include <opencv\cv.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
    Mat frame;
    namedWindow("video", 1);
    //VideoCapture cap("http://admin:admin@@65.114.169.151:88/videoMain.cgi?resolution=640x360&req_fps=50&.mjpg");
    VideoCapture cap("http://65.114.169.151:88/cgi-bin/CGIStream.cgi?cmd=GetMJStream&usr=admin&pwd=admin&.mjpg");

    if(!cap.isOpened())
    {
        cout<<"Camera not found"<<endl;
        getchar();
        return -1;
    }
    while ( cap.isOpened() )
    {
        cap >> frame;
        if(frame.empty()) break;

        imshow("video", frame);
        if(waitKey(30) >= 0) break;
    }   
    return 0;
}

