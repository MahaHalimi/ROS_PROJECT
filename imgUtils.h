//
// Created by fourniem on 12/01/20.
//

#ifndef TE1_IMGUTILS_H
#define TE1_IMGUTILS_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace cv;

class imgUtils {


public:
    imgUtils();
    static void  swapQuadrants(const Mat &magI);
    static Mat addPadding(const Mat &I);
    static Mat readImage(int argc, char *const *argv);
    static Mat getMagnitude(Mat &complexI);
    static Mat genSinusoidalFrame(int size);
    static Mat computeDFT(Mat &I, bool show);

private:


};


#endif //TE1_IMGUTILS_H
