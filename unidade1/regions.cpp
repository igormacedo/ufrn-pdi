#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;

  if (argc < 5) {
    cout << "Quatro argumentos numéricos são necessários" << endl;
    return 0;
  }

  int ax = atoi(argv[1]);
  int ay = atoi(argv[2]);
  int bx = atoi(argv[3]);
  int by = atoi(argv[4]);

  image = imread("media/biel.png",CV_LOAD_IMAGE_GRAYSCALE);
  if(!image.data)
    cout << "nao abriu biel.png" << endl;

  namedWindow("janela",WINDOW_AUTOSIZE);

  if (ax > 0 && ay > 0 && bx < image.rows && by < image.cols){
    for(int i=ax;i<bx;i++){
      for(int j=ay;j<by;j++){
        image.at<uchar>(i,j)= 255 - image.at<uchar>(i,j);
      }
    }
  }
  else{
    cout << "Argumentos devem ser de pontos dentro da imagem";
  }

  imshow("janela", image);  
  waitKey();

  return 0;
}
