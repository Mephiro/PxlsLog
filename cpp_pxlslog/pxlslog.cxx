#include "CanvasFunc.h"

int main(){

std::string userKeysName;
std::string logFileName;
std::string paletteFileName;
std::string canvasFileName;
pxls::canvas canvas = pxls::UI_init(&userKeysName,&logFileName,&paletteFileName, &canvasFileName);

cv::Mat inputImg = cv::imread(cv::samples::findFile(canvasFileName),cv::IMREAD_UNCHANGED);
pxls::drawing drawing(inputImg,canvas);
pxls::palette palette(paletteFileName);

std::vector<pxls::pxlsData> pxlsList;

pxls::threadingHash(8,&pxlsList,logFileName,userKeysName,&canvas);

if (canvas.printPixelNum() > 0){
    return 1;
}

drawing.drawImage(&pxlsList,palette,canvas);

drawing.showPlacemap();
return 0;
}