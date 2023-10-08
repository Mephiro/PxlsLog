#include "CanvasFunc.h"

int main(){

std::cout << "OpenCV discovery" << std::endl;

cv::Mat inputImg;

pxls::UI_init();

inputImg = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);

pxls::canvas canvas(true,false,true,false,"CEST","2023-08-31 00:00:00",2592000,3*3600);

std::string userKeysName = "./Mephi.txt";
std::string logFileName = "./pixels_c72.sanit.log";
std::string paletteFileName = "./palette_c73.txt";

std::vector<pxls::pxlsData> pxlsList;
pxls::rawData raw(userKeysName,logFileName);

pxls::threadingHash(1,&pxlsList,&raw,&canvas);

pxls::drawing drawing(inputImg,canvas);
pxls::palette palette(paletteFileName);

canvas.printPixelNum();

drawing.drawImage(&pxlsList,palette,canvas);

return 0;
}