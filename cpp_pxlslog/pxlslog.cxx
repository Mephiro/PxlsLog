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

    bool bHeatmap = false;
    std::string heatmap = argv[12];
    if (heatmap == "true"){
	bHeatmap = true;
    }
    bool bOverlay = false;
    std::string overlay = argv[11];
    if (overlay == "true"){
    	bOverlay = true;
    }
    bool bFullcanvas = false;
    std::string fullcanvas = argv[6];
    if (fullcanvas == "true"){
	bFullcanvas = true;
    }
    std::string timezone = argv[1];
    std::string Date = argv[2];
    std::string Time = argv[3];
    std::string beginDate = Date + " " + Time;
    uint64_t duration = uint64_t(atoi(argv[4]));
    uint interval = uint(atoi(argv[5]));
    canvas = pxls::canvas(crop,bTimelapse,bHeatmap,bOverlay,bFullcanvas,timezone,beginDate,duration,interval);
}else if (argc == 5){
    userKeysName = "userkey.txt";
    logFileName = "pixels.log";
    paletteFileName = "palette.txt";
    canvasFileName = "canvas-initial.png";

    bool bTimelapse = false;
    std::string timelapse = argv[1];
    if (timelapse == "true"){
        bTimelapse = true;
    }
    bool bHeatmap = false;
    std::string heatmap = argv[2];
    if (heatmap == "true"){
        bHeatmap = true;
    }
    bool bOverlay = false;
    std::string overlay = argv[3];
    if (overlay == "true"){
        bOverlay = true;
    }
    bool bFullcanvas = false;
    std::string fullcanvas = argv[4];
    if (fullcanvas == "true"){
        bFullcanvas = true;
    }

    canvas = pxls::canvas(bTimelapse,bHeatmap,bOverlay,bFullcanvas);
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
