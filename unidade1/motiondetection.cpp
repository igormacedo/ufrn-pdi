#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image, grey_image, eq_image, dst_image;
  int width, height;
  VideoCapture cap;
  vector<Mat> planes;
  Mat histR, old_hist; //, histB;
  int nbins = 64;
  float range[] = {0, 256};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;

  cap.open(0);
  
  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }
  
  width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  cout << "largura = " << width << endl;
  cout << "altura  = " << height << endl;

  int histw = nbins, histh = nbins/2;
  Mat histImgR(histh, histw, CV_8UC1, Scalar(0)); // modificado para criar um histograma em greyscale

  // faz uma vez para poder, calcular o primeiro dot product, dar continuidade dentro do loop
  cap >> image;
  cvtColor(image, grey_image, cv::COLOR_BGR2GRAY);
  calcHist(&grey_image, 1, 0, Mat(), histR, 1,
             &nbins, &histrange,
             uniform, acummulate);
  normalize(histR, histR, 0, histImgR.rows, NORM_MINMAX, -1, Mat());
  
  histR.copyTo(old_hist);

  while(1){
    cap >> image;

    cvtColor(image, grey_image, cv::COLOR_BGR2GRAY);

    calcHist(&grey_image, 1, 0, Mat(), histR, 1,
             &nbins, &histrange,
             uniform, acummulate);

    normalize(histR, histR, 0, histImgR.rows, NORM_MINMAX, -1, Mat());

    for(int i = 0; i< histR.rows; i++){
      for(int j = 0; j<histR.cols; j++){
        cout << +histR.at<uchar>(i,j) << endl;
      }
    }

    histImgR.setTo(Scalar(0));
    
    for(int i=0; i<nbins; i++){
      line(histImgR,
           Point(i, histh),
           Point(i, histh-cvRound(histR.at<float>(i))),
           Scalar(255), 1, 8, 0);
    }
    histImgR.copyTo(grey_image(Rect(0, 0 ,nbins, histh)));

    imshow("image", grey_image);
    if(waitKey(30) >= 0) break;
  }
  return 0;
}
