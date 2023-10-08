#include "CanvasFunc.h"

int main(){

std::cout << "OpenCV discovery" << std::endl;

cv::Mat in1Img;
in1Img = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);

pxls::canvas canvas(false,"CEST");
pxls::drawing drawing(in1Img,canvas);

std::string userKeysName = "./Mephi.txt";
std::string logFileName = "./pixels_c72.sanit.log";
std::string paletteFileName = "./palette_c73.txt";
std::vector<pxls::pxlsData> pxlsList;
pxls::palette palette(paletteFileName);

pxls::pxlsHash(userKeysName,logFileName,&pxlsList,&canvas);

canvas.printPixelNum();

return 0;

}