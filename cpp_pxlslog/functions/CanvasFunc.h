#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
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

class canvas {
    bool _timelapse = false;
    bool _heatmap = false;
    bool _overlay = false;
    bool _fullCanvas = false;

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
        std::cout<<"Canvas created!\n";
    }

    canvas(bool fullcanvas, std::string timezone){
        _fullCanvas = fullcanvas;
        _timezone = timezone;
        std::cout<<"Canvas created!\n";
    }

    canvas(bool timelapse, bool heatmap, bool overlay, bool fullcanvas, std::string timezone,
        std::string timeBegin, uint64_t captureDuration, uint64_t frameDuration){
        _timelapse = timelapse;
        _heatmap = heatmap;
        _overlay = overlay;
        _fullCanvas = fullcanvas;
        _timezone = timezone;
        _timeBegin = unixTimeConv(timeBegin,timezone);
        _captureDuration = captureDuration*1000;
        _frameDuration = frameDuration*1000;
        std::cout<<"Canvas created!\n";
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
    void printPixelNum(){
        std::cout<<"Pixel total canvas: "<<_nbPixelsTotal<<" | Undo total canvas: "<<_nbUndosTotal<<"\n";
        std::cout<<"Pixel user(s): "<<_nbPixelsKeys<<" | Undo user(s): "<<_nbUndosKeys<<"\n";
    }
};

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
        return _colors[colorIndex];
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
void pxlsHash(std::string userKeysFilename, std::string logFileFilename,
              std::vector<pxlsData> *pxlsList,canvas *canvas);

class drawing {
    cv::Mat _outImg;
    cv::Mat _bgImg;
    cv::Mat _fgImg;
    cv::Mat _heatImg;
    cv::VideoWriter _video;

public:
    drawing(cv::Mat inputImg, canvas canvas){
        inputImg.copyTo(_bgImg);
        _fgImg.create(inputImg.size(),inputImg.type());
        _heatImg.create(inputImg.size(),inputImg.type());
        _outImg.create(inputImg.size(),inputImg.type());
        if(canvas.timelapse()){
            int fourcc = cv::VideoWriter::fourcc('M', 'J','P','G');
            cv::Size videoSize(inputImg.size().width, inputImg.size().height);
            _video.open("timelapse.avi",cv::CAP_OPENCV_MJPEG,fourcc,10,videoSize,true);
        }
        std::cout<<"Brush created!\n";
    }

    ~drawing(){
        if(_video.isOpened()){
            _video.release();
        }
    }

    void drawImage(std::vector<pxlsData> *pxlsList, palette palette, canvas canvas){
        uint64_t currentTime;
        uint64_t frameStart;
        
        std::cout<<"Drawing in progress...\n";
        if(canvas.timeBegin() >= pxlsList->at(0).getUnixTime()){
            currentTime = pxlsList->at(0).getUnixTime();
            frameStart = canvas.timeBegin();
        }else{
            currentTime = canvas.timeBegin();
            frameStart = pxlsList->at(0).getUnixTime();
        }
        uint64_t captureStop = canvas.timeBegin()+canvas.captureDuration();
        while (currentTime <= frameStart){
            currentTime = drawFrame(pxlsList,palette,canvas,currentTime);
            if (canvas.timelapse()){
                maskOp(canvas);
            }
        }
        while (currentTime+canvas.frameDuration()<=captureStop){
            currentTime = drawFrame(pxlsList,palette,canvas,currentTime);
            if (canvas.timelapse()){
                maskOp(canvas);
            }
        }
        alphaMerge(_bgImg,_fgImg,_outImg);
        cv::imwrite("placemap.png",_outImg);
        std::cout<<"Drawing finished!\n";
    }

private:
    void maskOp(canvas canvas){
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
        _video.write(tempFrame);
    }

    void drawPxls(pxlsData pxlsData, palette palette){
        std::vector<int> currentCoord = pxlsData.getCoord();
        std::vector<uint> currentColor = palette.getColor(pxlsData.getColorIndex());
        cv::circle(_fgImg,cv::Point(currentCoord[0],currentCoord[1]),0,
        cv::Scalar(currentColor[0],currentColor[1],currentColor[2],255),cv::FILLED);
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
                if (pxlsList->at(i).getMyPixel()){drawPxls(pxlsList->at(i));}
            }
        }
        return currentTime+canvas.frameDuration();
    }
};
}

namespace hash{
    bool isMyPxls(const std::string digest,const std::string randomHash);
}