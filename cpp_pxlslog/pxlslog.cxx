#include "CanvasFunc.h"

int main(){

pxls::UI_init();
std::string userKeysName = "./Mephi.txt";
std::string logFileName = "./pixels_c72.sanit.log";
std::string paletteFileName = "./palette_c73.txt";

cv::Mat inputImg = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);
pxls::canvas canvas(true,true,true,false,"CEST","2023-08-31 00:00:00",2592000,3*3600);
//pxls::canvas canvas(false,"CEST");
pxls::drawing drawing(inputImg,canvas);
pxls::palette palette(paletteFileName);

std::vector<pxls::pxlsData> pxlsList;

pxls::threadingHash(6,&pxlsList,logFileName,userKeysName,&canvas);

canvas.printPixelNum();

drawing.drawImage(&pxlsList,palette,canvas);

return 0;
}