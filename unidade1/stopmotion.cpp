#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

Mat image, image32f, imagefiltered32f, imagefiltered;
Mat blended;
Mat imageTop;

Mat mask(3, 3, CV_32F), maskMedia;

char TrackbarName[50];

double alfa(int i, double a, double d, double p)
{
    return 0.5 * (tanh((i - p) / d) - tanh((i - a - p) / d));
}

int main(int argvc, char **argv)
{

    VideoCapture cap (argv[1]);

    if (!cap.isOpened()){
        cout << "Failed to open input file " << argv[1] << endl;
        exit(1);
    }

    VideoWriter out ("stopmotion.avi", CV_FOURCC('D','I','V','X'), 
                     cap.get(CV_CAP_PROP_FPS)/5,
                     Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), 
                     cap.get(CV_CAP_PROP_FRAME_HEIGHT)));


    while(1)
    {
        cap >> image;
        if(image.empty()) exit(0);

        //image = imread(argv[1]);
        blended = Mat::zeros(image.rows, image.cols, CV_8UC3);

        float media[] = {1, 1, 1,
                        1, 1, 1,
                        1, 1, 1};

        mask = Mat(3, 3, CV_32F, media);
        scaleAdd(mask, 1 / 9.0, Mat::zeros(3, 3, CV_32F), maskMedia);

        image.convertTo(image32f, CV_32F);
        filter2D(image32f, imagefiltered32f, image32f.depth(), maskMedia, Point(1, 1), 0);
        imagefiltered32f = abs(imagefiltered32f);
        imagefiltered32f.convertTo(imagefiltered, CV_8U);

        for (int i = 0; i < image.rows; i++)
        {
            double a = alfa(i, image.rows*0.15, 1, image.rows/2);
            addWeighted(image.row(i), a, imagefiltered.row(i), 1 - a, 0, blended.row(i));
        }

        out << blended;

    }

    return 0;
}
