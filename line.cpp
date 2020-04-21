#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "imgUtils.h"
#include <iostream>
#include <stdlib.h>
#include <list>
#include <limits>
#include <math.h>
#include <algorithm> 
#include "math.h"    
#include <iomanip>   


using namespace std;


void pathDetection(Mat &frame, Mat &frame_final, float& angle_value, float& abscissa_value){

    /// Matrix preparation for countours
    Mat frame_gray(frame.size(), CV_8UC1), frame_opened;
    cvtColor(frame, frame_gray, CV_RGB2GRAY);
    medianBlur ( frame_gray, frame_gray, 9 );
    threshold(frame_gray, frame_final, 150, 255, CV_THRESH_BINARY); 
    
    //dilate(frame_opened, frame_opened, getStructuringElement(MORPH_RECT, Size(50, 60)));

    /// Detection of contours
    Mat edges;
    Canny(frame_final, edges, 50, 200);
    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 20, 150, 250);
    float x1,x2,y1,y2;
    float n = 0,sumTheta =0,sumPosx=0;

    /// Finding center abscissa and orientation of the line
    for (size_t i=0; i<lines.size(); i++) {

        Vec4i l = lines[i];
        x1 = l[0],y1 = l[1],x2 = l[2],y2 = l[3];
        //cout << "angle = " << atan2(y2-y1,x2-x1)*180/M_PI << endl;
        //cout << "anglecondition = " << 70*M_PI/180 << endl;
        if ( -50*M_PI/180 < sawtooth(atan2(y2-y1,x2-x1)) < 50*M_PI/180 )
        {
            sumTheta += atan2(y2-y1,x2-x1) + M_PI/2;
            n += 1;
            sumPosx += (x1+x2)/2 ;  
        }
        line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, CV_AA);
    }

    sumTheta = sumTheta/n;
    sumPosx = sumPosx/n;
    abscissa_value = sumPosx - frame.cols/2 ;
    angle_value = sumTheta*180/M_PI;

    cout << "Orientation = " << angle_value << endl;
    cout << "Position moyenne = " << abscissa_value << endl;

   
}

void choosePath(float &angle, float &absDiff, float &wheel_rotation){
    // wheel_rotation should in [-10,10] deg
    float sign;
    wheel_rotation = 0.;
    if (absDiff > 0){
        sign = 1.;
    }else{
        sign = -1.;
    }
    // First : change the orientation of the line according to the robot's frame if the angle of difference is too important
    if(abs(angle)>5){
        wheel_rotation = sign*0.5*std::min((float)20.,angle);
        // Second : if the line is straight in the robots's frame, change the rotation accordingly to the line position (must reach 0)
        if(abs(absDiff)>10){
            wheel_rotation = 0.05*std::min((float)200.,absDiff);
        }
    }
}



int main(int argc, char **argv){

    Mat Image = cv::imread(argv[1], 1); //loads color if it is available
    if (Image.empty()) {
        cerr << "Error opening image" << endl;
        exit(EXIT_FAILURE);
    }
    imshow("Image d'origine",Image);

    Mat PathImage;
    float angle, absDiff;
    pathDetection(Image, PathImage, angle, absDiff);
    imshow("Image avec contour ",Image);
    imshow("image binarise avec contours",PathImage);

     Mat frame_opened,frame_gray;
    
    erode(PathImage, frame_opened, getStructuringElement(MORPH_RECT, Size(8, 8)));
    dilate(frame_opened, frame_gray, getStructuringElement(MORPH_RECT, Size(50, 60)));
    
    imshow("image dilaté",frame_gray);
    
    float wheel_rotation;
    choosePath(angle, absDiff, wheel_rotation);
    cout << "The wheels shoud rotate : " << setprecision(2) << wheel_rotation  << " degree(s)." << endl;

    waitKey(0);
    
    return 0;
}