#include "CanvasFunc.h"

int main(){

std::cout << "OpenCV discovery" << std::endl;

cv::Mat inputImg;
inputImg = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);

pxls::canvas canvas(true,true,true,false,"CEST","2023-08-31 00:00:00",2592000,3*3600);
pxls::drawing drawing(inputImg,canvas);

std::string userKeysName = "./Mephi.txt";
std::string logFileName = "./pixels_c72.sanit.log";
std::string paletteFileName = "./palette_c73.txt";
std::vector<pxls::pxlsData> pxlsList;
pxls::palette palette(paletteFileName);

pxls::pxlsHash(userKeysName,logFileName,&pxlsList,&canvas);

canvas.printPixelNum();

drawing.drawImage(&pxlsList,palette,canvas);

return 0;

}