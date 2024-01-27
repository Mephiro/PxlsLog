#include "CanvasFunc.h"

int main(){
/*
std::string input = "hello world";
std::string output = "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9";

bool result1 = hash::isMyPxls2(input,output);

bool result2 = hash::isMyPxls(input,output);
*/

std::string userKeysName;
std::string logFileName;
std::string paletteFileName;
std::string canvasFileName;
pxls::canvas canvas = pxls::UI_init(&userKeysName,&logFileName,&paletteFileName, &canvasFileName);

cv::Mat inputImg = cv::imread(cv::samples::findFile(canvasFileName),cv::IMREAD_UNCHANGED);
pxls::drawing drawing(inputImg,canvas);
pxls::palette palette(paletteFileName);

std::vector<pxls::pxlsData> pxlsList;

pxls::threadingHash(4,&pxlsList,logFileName,userKeysName,&canvas);

if (canvas.printPixelNum() > 0){
    return 1;
}

drawing.drawImage(&pxlsList,palette,canvas);

drawing.showPlacemap();
return 0;
}