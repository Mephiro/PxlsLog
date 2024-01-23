#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <openssl/evp.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define SHA256_DIGEST_LENGTH 32

namespace pxls{

struct RGBA {
uchar blue;
uchar green;
uchar red;
uchar alpha;
};

std::vector<uint> hexToBGR(char const *hexColor);
std::vector<std::string> lineParser(std::string line,char separator);
uint64_t unixTimeConv(std::string strTime, std::string timezone);
std::vector<std::string> subVector(const std::vector<std::string> *vec, size_t startIdx, size_t endIdx);

class canvas {
    bool _timelapse = false;
    bool _heatmap = false;
    bool _overlay = false;
    bool _fullCanvas = false;

    std::vector<uint> _crop = {0,0,0,0};

    uint64_t _timeBegin = 0;
    uint64_t _captureDuration = 0;
    uint64_t _frameDuration = 0;

    std::string _timezone = "CEST";

    uint64_t _nbPixelsTotal = 0;
    uint64_t _nbUndosTotal = 0;
    uint64_t _nbPixelsKeys = 0;
    uint64_t _nbUndosKeys = 0;

public:
    canvas(){
        //Empty constructor
    }

    canvas(bool fullcanvas, bool overlay, std::string timezone){
        _fullCanvas = fullcanvas;
        _timezone = timezone;
        _overlay = overlay;
        std::cout<<"Canvas created!\n";
    }

    canvas(std::vector<uint> crop, bool timelapse, bool heatmap, bool overlay, bool fullcanvas, std::string timezone,
        std::string timeBegin, uint64_t captureDuration, uint64_t frameDuration){
        _timelapse = timelapse;
        _heatmap = heatmap;
        _overlay = overlay;
        _fullCanvas = fullcanvas;
        _timezone = timezone;
        _timeBegin = unixTimeConv(timeBegin,timezone);
        _captureDuration = captureDuration*1000;
        _frameDuration = frameDuration*1000;
        _crop = crop;
        std::cout<<"Canvas created!\n";
    }

    std::vector<uint> cropSize(){
        return _crop;
    }
    std::string timezone(){
        return _timezone;
    }
    bool timelapse(){
        return _timelapse;
    }
    bool heatmap(){
        return _heatmap;
    }
    bool overlay(){
        return _overlay;
    }
    bool fullCanvas(){
        return _fullCanvas;
    }
    uint64_t timeBegin(){
        return _timeBegin;
    }
    uint64_t captureDuration(){
        return _captureDuration;
    }
    uint64_t frameDuration(){
        return _frameDuration;
    }
    void setTimes(uint64_t timeBegin,uint64_t captureDuration,uint64_t frameDuration){
        _timeBegin = timeBegin;
        _captureDuration = captureDuration;
        _frameDuration = frameDuration;
    }
    void addPixelsTot(){
        _nbPixelsTotal +=1;
    }
    void addUndosTot(){
        _nbUndosTotal +=1;
    }
    void addPixelsKey(){
        _nbPixelsKeys +=1;
    }
    void addUndosKey(){
        _nbUndosKeys +=1;
    }
    std::vector<uint64_t> getPixelNum(){
        return {_nbPixelsTotal,_nbUndosTotal,_nbPixelsKeys,_nbUndosKeys};
    }
    int printPixelNum(){
        if (_nbPixelsKeys == 0){
            std::cerr<<"Error with the given log key !\nStopping...";
            return 1;
        }
        std::cout<<"Pixel total canvas: "<<_nbPixelsTotal<<" | Undo total canvas: "<<_nbUndosTotal<<"\n";
        std::cout<<"Pixel user(s): "<<_nbPixelsKeys<<" | Undo user(s): "<<_nbUndosKeys<<"\n";
        return 0;
    }
};

canvas UI_init(std::string *userkeyFilename, std::string *logFilename, std::string *paletteFilename, std::string *canvasFileName);

class palette {
    std::vector<std::string> _names;
    std::vector<std::vector<uint>> _colors;
    uint _paletteSize = 0;

public:
    palette(std::string paletteFilename){
        std::string paletteLine;
        std::fstream paletteFile(paletteFilename,std::ios::in);
        while (std::getline(paletteFile,paletteLine))
        {
            std::vector<uint> color;
            std::vector<std::string> parse = lineParser(paletteLine,' ');
            color.push_back(stoi(parse[2]));
            color.push_back(stoi(parse[1]));
            color.push_back(stoi(parse[0]));
            _colors.push_back(color);
            _names.push_back(parse[3]);
        }
        paletteFile.close();
        _paletteSize = _colors.size();
        std::cout<<"Palette created!\n";
    }
    
    std::string getColorName(int colorIndex){
        return _names[colorIndex];
    }
    std::vector<uint> getColor(int colorIndex){
        if (colorIndex == -1){
            return {0,0,0,0};
        }
        std::vector<uint> currentColor = _colors[colorIndex];
        currentColor.push_back(255);
        return currentColor;
    }
    int getPaletteSize(){
        return _paletteSize;
    }
};

class pxlsData {
    uint64_t _unixDate = 0;
    std::string _date = "";
    std::string _randomHash = "";
    std::vector<int> _xy = {0,0};
    int _colorIndex = 0;
    std::string _action = "";
    std::vector<uint> _heatmapColor = {0,0,0};
    bool _mypixel = false;

public:
    pxlsData(std::string inLine,canvas *canvas){
        std::vector<std::string> parse;
        parse = lineParser(inLine,'\t');
        _date = parse[0];
        _unixDate = unixTimeConv(parse[0],canvas->timezone());
        _randomHash = parse[1];
        _xy = {std::stoi(parse[2]),std::stoi(parse[3])};
        _colorIndex = std::stoi(parse[4]);
        _action = parse[5];
    }

    bool operator < (const pxlsData& pxlsData){
        return (_unixDate < pxlsData._unixDate);
    }

    bool operator < (const uint64_t& unixDate){
        return (_unixDate < unixDate);
    }

    bool operator == (const pxlsData& pxlsData){
        return ((_randomHash == pxlsData._randomHash)
                && (_unixDate == pxlsData._unixDate));
    }

    std::string pxlsDigest(std::string userKey){
        std::stringstream digest;
        digest<<_date<<','<<_xy[0]<<','<<_xy[1]<<','<<_colorIndex<<','<<userKey;
        return digest.str();
    }

    void countPxls(canvas *canvas){
        bool placed = (_action == "user place");
        if(_mypixel){
            if (placed){
                canvas->addPixelsKey();
                canvas->addPixelsTot();
            }else{
                canvas->addUndosKey();
                canvas->addUndosTot();
            }
        }else{
            if (placed){
                canvas->addPixelsTot();
            }else{
                canvas->addUndosTot();
            }
        }
    }

    void setMyPixel(bool mypixel){
        _mypixel = mypixel;
    }

    void addHeatColor(std::string heatColor){
        _heatmapColor = hexToBGR(heatColor.c_str());
    }

    std::string getRandomHash(){
        return _randomHash;
    }
    uint64_t getUnixTime(){
        return _unixDate;
    }
    std::string getTime(){
        return _date;
    }
    std::vector<int> getCoord(){
        return _xy;
    }
    int getColorIndex(){
        return _colorIndex;
    }
    std::string getAction(){
        return _action;
    }
    std::vector<uint> getHeatColor(){
        return _heatmapColor;
    }
    bool getMyPixel(){
        return _mypixel;
    }
};

int lowerBoundIdx(std::vector<pxlsData> *pxlsList, uint64_t unixTime);
void alphaMerge(cv::Mat src1, cv::Mat src2, cv::Mat dst);
void brigthnessFunc(cv::Mat src, cv::Mat dst, const float factor);
void alphaFunc(cv::Mat src, cv::Mat dst, const float factor);
void pxlsHash(std::vector<pxlsData> *pxlsList, std::string pxlsLogName, std::vector<std::string> userKeys,canvas *canvas, int lineStart, int lineStop);
void threadingHash(int nbCore, std::vector<pxlsData> *PxlsList, const std::string pxlsLogName,const std::string keysLogName, canvas *canvas);

class drawing {
    cv::Mat _outImg;
    cv::Mat _bgImg;
    cv::Mat _fgImg;
    cv::Mat _heatImg;
    cv::Range xcrop;
    cv::Range ycrop;

public:
    drawing(cv::Mat inputImg, canvas canvas){
        std::vector<uint> cropSize = canvas.cropSize();
        if ((cropSize[0]-cropSize[1] == 0) || (cropSize[2]-cropSize[3] == 0)){
            xcrop = cv::Range(0,inputImg.size().width);
            ycrop = cv::Range(0,inputImg.size().height);
        }else{
            xcrop = cv::Range(cropSize[0],cropSize[1]);
            ycrop = cv::Range(cropSize[2],cropSize[3]);
        }

        inputImg.copyTo(_bgImg);
        _fgImg.create(inputImg.size(),inputImg.type());
        _heatImg.create(inputImg.size(),inputImg.type());
        _outImg.create(inputImg.size(),inputImg.type());
        std::cout<<"Brush created!\n";
    }

    void drawImage(std::vector<pxlsData> *pxlsList, palette palette, canvas canvas){
        uint64_t currentTime;
        uint64_t frameStart;
        uint64_t captureStop;
        cv::VideoWriter video;
        int fourcc = cv::VideoWriter::fourcc('M','J','P','G');
        cv::Size videoSize(xcrop.size(),ycrop.size());
        if(canvas.timelapse()){
            video.open("timelapse.avi",cv::CAP_OPENCV_MJPEG,fourcc,10,videoSize,true);
        }
        
        std::cout<<"Drawing in progress...\n";
        if (canvas.timeBegin() == 0){
            canvas.setTimes(pxlsList->at(0).getUnixTime(),
            pxlsList->back().getUnixTime()-pxlsList->at(0).getUnixTime(),
            36000);
        }
        captureStop = canvas.timeBegin()+canvas.captureDuration();
        if(canvas.timeBegin() >= pxlsList->at(0).getUnixTime()){
            currentTime = pxlsList->at(0).getUnixTime();
            frameStart = canvas.timeBegin();
        }else{
            currentTime = canvas.timeBegin();
            frameStart = canvas.timeBegin();
        }

        while (currentTime <= frameStart){
            currentTime = drawFrame(pxlsList,palette,canvas,currentTime);
            progress(currentTime,canvas.timeBegin(),captureStop);
        }
        while (currentTime+canvas.frameDuration()<=captureStop){
            currentTime = drawFrame(pxlsList,palette,canvas,currentTime);
            progress(currentTime,canvas.timeBegin(),captureStop);
            if (canvas.timelapse()){
                video.write(maskOp(canvas));
            }
        }

        if(canvas.overlay()){
            alphaMerge(_bgImg,_fgImg,_outImg);
        }else{
            _outImg = _fgImg;
        }

        cv::imwrite("placemap.png",_outImg);
        std::cout<<"\nDrawing finished!\n\n";
        if (canvas.timelapse()){
            //system("./avi2mp4.sh");
            video.release();
        }
    }

    void showPlacemap(){
        cv::resize(_outImg,_outImg,_outImg.size()/2);
        cv::imshow("Output Placemap",_outImg);
        std::cout << "\nPress Esc to close.\n";
        cv::waitKey();
    }

private:

    void progress(u_int64_t current, u_int64_t initial, u_int64_t target){
        std::cout << "Progress :" << 100*(current - initial)/(target - initial) << "%\r";
    }

    cv::Mat maskOp(canvas canvas){
        cv::Mat tempFrame(_bgImg.size(),_bgImg.type());
        if(canvas.overlay()){
            alphaMerge(_bgImg,_fgImg,tempFrame);
            if(canvas.heatmap()){
                brigthnessFunc(tempFrame,tempFrame,0.3);
                alphaMerge(tempFrame,_heatImg,tempFrame);
                alphaFunc(_heatImg,_heatImg,0.8);
            }
        }else{
            if(canvas.heatmap()){
                _bgImg.copyTo(tempFrame);
                alphaFunc(tempFrame,tempFrame,0.3);
                alphaMerge(tempFrame,_heatImg,tempFrame);
            }else{
                _fgImg.copyTo(tempFrame);
            }
        }
        cv::cvtColor(tempFrame,tempFrame,cv::COLOR_BGRA2RGBA);
        
        tempFrame = tempFrame(ycrop,xcrop);
        return tempFrame;
    }

    void drawPxls(pxlsData pxlsData, palette palette){
        std::vector<int> currentCoord = pxlsData.getCoord();
        std::vector<uint> currentColor = palette.getColor(pxlsData.getColorIndex());
        cv::circle(_fgImg,cv::Point(currentCoord[0],currentCoord[1]),0,
        cv::Scalar(currentColor[0],currentColor[1],currentColor[2],currentColor[3]),cv::FILLED);
    }

    void drawPxls(pxlsData pxlsData){
        std::vector<int> currentCoord = pxlsData.getCoord();
        std::vector<uint> currentColor = pxlsData.getHeatColor();
        cv::circle(_heatImg,cv::Point(currentCoord[0],currentCoord[1]),0,
        cv::Scalar(currentColor[0],currentColor[1],currentColor[2],255),cv::FILLED);
    }

    uint64_t drawFrame(std::vector<pxlsData> *pxlsList, palette palette, canvas canvas, uint64_t currentTime){
        int idxStart = lowerBoundIdx(pxlsList,currentTime);
        int idxStop = lowerBoundIdx(pxlsList,currentTime+canvas.frameDuration());
        for (int i = idxStart; i < idxStop; i++){
            drawPxls(pxlsList->at(i),palette);
            if(canvas.heatmap()){
                if (pxlsList->at(i).getMyPixel()){
                    drawPxls(pxlsList->at(i));
                }
            }
        }
        return currentTime+canvas.frameDuration();
    }
};
}

namespace hash{
    bool isMyPxls(const std::string digest,const std::string randomHash);
}
