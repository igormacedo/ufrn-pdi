#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

double altura;
int altura_slider = 0;
int altura_slider_max = 100;

double decaimento;
int decaimento_slider = 0;
int decaimento_slider_max = 100;

double posicao;
int posicao_slider = 0;
int posicao_slider_max = 100;

Mat image, image32f, imagefiltered32f, imagefiltered;
Mat blended;
Mat imageTop;

Mat mask(3, 3, CV_32F), maskMedia;

char TrackbarName[50];

double alfa(int i, double a, double d, double p)
{
    return 0.5 * (tanh((i - p) / d) - tanh((i - a - p) / d));
}

void on_trackbar_altura(int, void *)
{
    for (int i = 0; i < image.rows; i++)
    {
        double a = alfa(i, altura_slider, decaimento_slider+1, posicao_slider);
        addWeighted(image.row(i), a, imagefiltered.row(i), 1 - a, 0, blended.row(i));
    }
    imshow("tiltshift", blended);
}

void on_trackbar_decaimento(int, void *)
{
    on_trackbar_altura(altura_slider, 0);
}

void on_trackbar_posicao(int, void *)
{
    on_trackbar_altura(altura_slider, 0);
}

int main(int argvc, char **argv)
{
    image = imread(argv[1]);
    blended = Mat::zeros(image.rows, image.cols, CV_8UC3);
    altura_slider_max = image.rows;
    posicao_slider_max = image.rows;

    float media[] = {1, 1, 1,
                     1, 1, 1,
                     1, 1, 1};

    mask = Mat(3, 3, CV_32F, media);
    scaleAdd(mask, 1 / 9.0, Mat::zeros(3, 3, CV_32F), maskMedia);

    image.convertTo(image32f, CV_32F);
    filter2D(image32f, imagefiltered32f, image32f.depth(), maskMedia, Point(1, 1), 0);
    imagefiltered32f = abs(imagefiltered32f);
    imagefiltered32f.convertTo(imagefiltered, CV_8U);

    namedWindow("tiltshift", 1);

    sprintf(TrackbarName, "Altura (0-%d)", altura_slider_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &altura_slider,
                   altura_slider_max,
                   on_trackbar_altura);
    on_trackbar_altura(altura_slider, 0);

    sprintf(TrackbarName, "Decaimento (0-%d)", decaimento_slider_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &decaimento_slider,
                   decaimento_slider_max,
                   on_trackbar_decaimento);
    on_trackbar_decaimento(decaimento_slider, 0);

    sprintf(TrackbarName, "Posição (0-%d)", posicao_slider_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &posicao_slider,
                   posicao_slider_max,
                   on_trackbar_posicao);
    on_trackbar_posicao(posicao_slider, 0);

    waitKey(0);

    imwrite( "blended.png", blended );
    return 0;
}
