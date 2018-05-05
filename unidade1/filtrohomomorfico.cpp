#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define RADIUS 20

using namespace cv;
using namespace std;

int gl_slider = 2;
int gl_slider_max = 10;

int gh_slider = 20;
int gh_slider_max = 50;

int c_slider = 50;
int c_slider_max = 100;

int d0_slider = 5;
int d0_slider_max = 200;

Mat imageOriginal;
Mat padded;
Mat_<float> realInput, zeros;
Mat complexImage;
Mat filter, tmp;
vector<Mat> planos;

int dft_M, dft_N;

// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image){
  Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para
  // evitar cópias de tamanho desigual
  image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
  int cx = image.cols/2;
  int cy = image.rows/2;
  
  // reorganiza os quadrantes da transformada
  // A B   ->  D C
  // C D       B A
  A = image(Rect(0, 0, cx, cy));
  B = image(Rect(cx, 0, cx, cy));
  C = image(Rect(0, cy, cx, cy));
  D = image(Rect(cx, cy, cx, cy));

  // A <-> D
  A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);

  // C <-> B
  C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}

cv::Mat create_homomorfic_filter(cv::Size paddedSize, double gl, double gh, double c, double d0){
  Mat filter = Mat(paddedSize, CV_32FC2, Scalar(0));
  Mat tmp = Mat(dft_M, dft_N, CV_32F);

  for(int i=0; i < tmp.rows; i++){
    for(int j=0; j < tmp.cols; j++){
        float coef = (i-dft_M/2)*(i-dft_M/2)+(j-dft_N/2)*(j-dft_N/2);
        tmp.at<float> (i,j) = (gh-gl)*(1.0 - (float)exp(-(c*coef/(d0*d0)))) + gl;
    }
  }

  // cria a matriz com as componentes do filtro e junta
  // ambas em uma matriz multicanal complexa
  Mat comps[]= {tmp, tmp};
  merge(comps, 2, filter);
  return filter;
}

void on_trackbar_move(int, void*){
  // limpa o array de matrizes que vao compor a
  // imagem complexa
  planos.clear();

  // cria a compoente real
  realInput = Mat_<float>(padded);

  // insere as duas componentes no array de matrizes
  planos.push_back(realInput);
  planos.push_back(zeros);

  // combina o array de matrizes em uma unica
  // componente complexa
  merge(planos, complexImage);

  // // calcula o dft
  // dft(complexImage, complexImage);

  // // realiza a troca de quadrantes
  // deslocaDFT(complexImage);

  // // aplica o filtro frequencial
  // Mat filter = create_homomorfic_filter(padded.size(),gl_slider,gh_slider,c_slider,d0_slider);
  // mulSpectrums(complexImage,filter,complexImage,0);

  // // troca novamente os quadrantes
  // deslocaDFT(complexImage);

  // // calcula a DFT inversa
  // idft(complexImage, complexImage);

  // // limpa o array de planos
  // planos.clear();

  // // separa as partes real e imaginaria da
  // // imagem filtrada
  // split(complexImage, planos);

  // // normaliza a parte real para exibicao
  // normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
  // imshow("Homomorphic Filter", planos[0]);
}

int main(int argc, char** argv){

  Mat imageOriginal = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  imshow("Original", imageOriginal);

  // identifica os tamanhos otimos para
  // calculo do FFT
  int dft_M = getOptimalDFTSize(imageOriginal.rows);
  int dft_N = getOptimalDFTSize(imageOriginal.cols);

  // realiza o padding da imagem
  Mat padded;
  copyMakeBorder(imageOriginal, padded, 0,
                 dft_M - imageOriginal.rows, 0,
                 dft_N - imageOriginal.cols,
                 BORDER_CONSTANT, Scalar::all(0));

  // parte imaginaria da matriz complexa (preenchida com zeros)
  zeros = Mat_<float>::zeros(padded.size());

  // prepara a matriz complexa para ser preenchida
  complexImage = Mat(padded.size(), CV_32FC2, Scalar(0));

  // a função de transferência (filtro frequencial) deve ter o
  // mesmo tamanho e tipo da matriz complexa
  filter = complexImage.clone();

  // cria uma matriz temporária para criar as componentes real
  // e imaginaria do filtro ideal
  tmp = Mat(dft_M, dft_N, CV_32F);

  char TrackbarName[50];

  namedWindow("Homomorphic Filter", WINDOW_NORMAL);

  sprintf( TrackbarName, "Gamma L x %d", gl_slider_max );
  createTrackbar( TrackbarName, "Homomorphic Filter", &gl_slider, gl_slider_max, on_trackbar_move);

  sprintf( TrackbarName, "Gamma H x %d", gh_slider_max );
  createTrackbar( TrackbarName, "Homomorphic Filter", &gh_slider, gh_slider_max, on_trackbar_move);

  sprintf( TrackbarName, "C x %d", c_slider_max);
  createTrackbar( TrackbarName, "Homomorphic Filter", &c_slider, c_slider_max, on_trackbar_move);

  sprintf( TrackbarName, "Cutoff Frequency x %d", d0_slider_max );
  createTrackbar( TrackbarName, "Homomorphic Filter", &d0_slider, d0_slider_max, on_trackbar_move);

  on_trackbar_move(100, NULL);

  waitKey(0);
  return 0;
}
