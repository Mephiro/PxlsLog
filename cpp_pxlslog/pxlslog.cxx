#include "CanvasFunc.h"

int main(int argc, char *argv[]){

std::string userKeysName;
std::string logFileName;
std::string paletteFileName;
std::string canvasFileName;
pxls::canvas canvas;

if (argc == 13){
    userKeysName = "userkey.txt";
    logFileName = "pixels.log";
    paletteFileName = "palette.txt";
    canvasFileName = "canvas-initial.png";
    std::vector<uint> crop = {uint(atoi(argv[7])), uint(atoi(argv[8])), uint(atoi(argv[9])), uint(atoi(argv[10]))};
    bool bTimelapse = true;
    bool bHeatmap = argv[12];
    bool bOverlay = argv[11];
    bool bFullcanvas = argv[6];
    std::string timezone = argv[1];
    std::string Date = argv[2];
    std::string Time = argv[3];
    std::string beginDate = Date + " " + Time;
    uint64_t duration = uint64_t(atoi(argv[4]));
    uint interval = uint(atoi(argv[5]));
    canvas = pxls::canvas(crop,bTimelapse,bHeatmap,bOverlay,bFullcanvas,timezone,beginDate,duration,interval);
}else{
    canvas = pxls::UI_init(&userKeysName,&logFileName,&paletteFileName, &canvasFileName);
}

cv::Mat inputImg = cv::imread(cv::samples::findFile(canvasFileName),cv::IMREAD_UNCHANGED);
pxls::drawing drawing(inputImg,canvas);
pxls::palette palette(paletteFileName);

std::vector<pxls::pxlsData> pxlsList;

pxls::threadingHash(1,&pxlsList,logFileName,userKeysName,&canvas);

if (canvas.printPixelNum() > 0){
    return 1;
}

drawing.drawImage(&pxlsList,palette,canvas);

//drawing.showPlacemap();
return 0;
}
