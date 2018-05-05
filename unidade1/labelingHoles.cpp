#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image, imgholes, imgregions, imgaux;
  int width, height;
  int nobjects;
  
  CvPoint p;
  image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  
  if(!image.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }
  width=image.cols;
  height=image.rows;

  p.x=0;
  p.y=0;

  // Elimina elementos que tocam as bordas superior e inferior
  for(int i=0; i<height; i=i+height-1){
    for(int j=0; j<width; j++){
      if(image.at<uchar>(i,j) == 255){
        p.x=j;
        p.y=i;
        floodFill(image,p,0);
      }
	  }
  }

  // Elimina elementos que tocam as bordas laterais
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j = j+width-1){
      if(image.at<uchar>(i,j) == 255){
        p.x=j;
        p.y=i;
        floodFill(image,p,0);
      }
	  }
  }

  image.copyTo(imgholes);
  image.copyTo(imgregions);
  image.copyTo(imgaux);

  // Preenche background com fundo branco
  p.x=0;
  p.y=0;
  floodFill(imgaux,p,255);

  // HOLES DETECTION
  // detectar pontos pretos em uma imagem e preencher com identificador o pixel (anterior)
  // da outra imagem
  nobjects=255; // inicia em 255 para facilitar visualização
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      if(imgaux.at<uchar>(i,j) == 0){
        // achou uma região preta
        if(imgholes.at<uchar>(i,j-1) == 255){
          nobjects -= 15; // diminui 15 para facilitar visualização
          p.x=j-1;
          p.y=i;
          floodFill(imgholes,p,nobjects);
        }        
      }
	  }
  }

  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      if(imgholes.at<uchar>(i,j) == 255){
        p.x=j;
        p.y=i;
        floodFill(imgholes,p,0);
      }
    }
  }

  // REGIONS WITHOUT HOLES DETECTION

  nobjects=255;
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      if(imgholes.at<uchar>(i,j) != 255 && imgholes.at<uchar>(i,j) != 0 && imgregions.at<uchar>(i,j) == 255){
         // diminui 15 para facilitar visualização
        p.x=j;
        p.y=i;
        floodFill(imgregions,p,0);
      }
      if(imgregions.at<uchar>(i,j) == 255){
        nobjects -= 10;
        p.x=j;
        p.y=i;
        floodFill(imgregions,p,nobjects);
      }
    }
  }

  imwrite("media/labelingHolesRegions.png", imgregions);
  imwrite("media/labelingHolesDetected.png", imgholes);
  waitKey();
  return 0;
}
