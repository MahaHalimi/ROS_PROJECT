//
// Created by fourniem on 12/01/20.
//

#include <iostream>
#include "imgUtils.h"

#define PI 3.141592653589

#define fx 0.00003
#define fy 0.08


using namespace cv;

imgUtils::imgUtils() = default;





void imgUtils::swapQuadrants(const Mat &magI) {// rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols / 2;
    int cy = magI.rows / 2;

    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

Mat imgUtils::addPadding(const Mat &I) {
    Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize(I.rows);
    int n = getOptimalDFTSize(I.cols); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    return padded;
}

Mat imgUtils::readImage(int argc, char *const *argv) {
    const char *filename = argc >= 2 ? argv[1] : "../lena.png";


    Mat I = imread(filename, IMREAD_GRAYSCALE);
    if (I.empty()) {
        std::cerr << "Error opening image" << std::endl;
        exit(EXIT_FAILURE);
    }
    return I;
}

Mat imgUtils::getMagnitude(Mat &complexI) {
    Mat planes[] = {Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };
    split(complexI, planes);
    Mat res = Mat::zeros(complexI.size(), CV_32F);
    magnitude(planes[0], planes[1], res);

    res += Scalar::all(1);

    return res;
}




// this class is for OpenCV ParallelLoopBody
class Parallel_pixel_opencv : public ParallelLoopBody
{
private:
    float *p ;
    int size;
public:
    Parallel_pixel_opencv(float * ptr, int i_size ) {
        p = ptr;
        size = i_size;
    }


    virtual void operator()( const Range &r ) const
    {
        register int j;
        for ( register int i = r.start; i != r.end; ++i)
        {
            j = i % size;
            p[i] = (float)cos( 2*PI*fx*i + 2*PI*j*fy)  ;
        }
    }
};


Mat imgUtils::genSinusoidalFrame(int size) {
    Mat sinFrame = Mat::zeros(size, size, CV_32F);
    auto* p3 = (float*) sinFrame.data ;

    // timing ParallelLoopBody start
    parallel_for_( Range(0,size * size) , Parallel_pixel_opencv(p3, size)) ;
    return sinFrame;
}

Mat imgUtils::computeDFT(Mat &I, bool show) {

    Mat padded = imgUtils::addPadding(I);

    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

    dft(complexI, complexI);            // this way the result may fit in the source matrix


    // compute the magnitude and switch to logarithmic scale
    Mat magI = imgUtils::getMagnitude(complexI);
    log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    imgUtils::swapQuadrants(magI);

    normalize(magI, magI, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a
    // viewable image form (float between values 0 and 1).

    Mat display;
    hconcat(I,magI, display);

    if (show) imshow("Input Image and DFT", display);    // Show the result
//    if (show) imshow("spectrum magnitude", magI);

    return magI;

}
