#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace cv;
using namespace std;

#define STEP 5
#define JITTER 3

int top_slider = 10;
int top_slider_max = 200;

int pointradi_slider = 3;
int pointradi_slider_max = 10;

int pointradicanny_slider = 3;
int pointradicanny_slider_max = 10;

char TrackbarName[50];

Mat image_gray, image_color;
Mat border, frame, points;

int width, height, gray;
int x, y;

vector<int> yrange;
vector<int> xrange;

void on_trackbar_canny(int, void *)
{
  Canny(image_gray, border, top_slider, 3 * top_slider);

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

  for (uint i = 0; i < xrange.size(); i++)
  {
    xrange[i] = xrange[i] * STEP + STEP / 2;
  }

  for (uint i = 0; i < yrange.size(); i++)
  {
    yrange[i] = yrange[i] * STEP + STEP / 2;
  }

  points = Mat(height, width, CV_8UC3, Scalar(255, 255, 255));

  for (auto i : xrange)
  {
    random_shuffle(yrange.begin(), yrange.end());
    for (auto j : yrange)
    {
      x = i + rand() % (2 * JITTER) - JITTER + 1;
      y = j + rand() % (2 * JITTER) - JITTER + 1;
      Vec3b color = image_color.at<Vec3b>(x, y);
      circle(points, cv::Point(y, x), pointradi_slider + rand() % 4, Scalar(color), -1, CV_AA);
    }
  }

  for (auto i : xrange)
  {
    random_shuffle(yrange.begin(), yrange.end());
    for (auto j : yrange)
    {
      if (border.at<uchar>(i,j) == 255)
      {
        Vec3b color = image_color.at<Vec3b>(i, j);
        circle(points, cv::Point(j, i), pointradicanny_slider + rand() % 2, Scalar(color), -1, CV_AA);
      }
    }
  }

  imshow("CannyPontos", points);
}

int main(int argc, char **argv)
{

  srand(time(0));
  //image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  image_color = imread(argv[1]);

  if (!image_color.data)
  {
    cout << "nao abriu" << argv[1] << endl;
    cout << argv[0] << " imagem.jpg";
    exit(0);
  }

  cvtColor(image_color, image_gray, CV_RGB2GRAY);

  width = image_color.size().width;
  height = image_color.size().height;

  xrange.resize(height / STEP);
  yrange.resize(width / STEP);

  sprintf(TrackbarName, "Threshold inferior %d", top_slider_max);

  namedWindow("CannyPontos", 1);
  createTrackbar(TrackbarName, "CannyPontos",
                 &top_slider,
                 top_slider_max,
                 on_trackbar_canny);
  createTrackbar("Point Radius", "CannyPontos",
                 &pointradi_slider,
                 pointradi_slider_max,
                 on_trackbar_canny);
  createTrackbar("Canny point radius", "CannyPontos",
                 &pointradicanny_slider,
                 pointradicanny_slider_max,
                 on_trackbar_canny);

  on_trackbar_canny(top_slider, 0);

  waitKey();
  return 0;
}
