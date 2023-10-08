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
    }

    canvas(bool fullcanvas, std::string timezone){
        _fullCanvas = fullcanvas;
        _timezone = timezone;
    }

    canvas(bool timelapse, bool heatmap, bool overlay, bool fullcanvas, std::string timezone,
        std::string timeBegin, uint64_t captureDuration, uint64_t frameDuration){
        _timelapse = timelapse;
        _heatmap = heatmap;
        _overlay = overlay;
        _fullCanvas = fullcanvas;
        _timezone = timezone;
        _timeBegin = unixTimeConv(timeBegin,timezone);
        _captureDuration = captureDuration;
        _frameDuration = frameDuration;
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
    bool timeBegin(){
        return _timeBegin;
    }
    bool captureDuration(){
        return _captureDuration;
    }
    bool frameDuration(){
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
        std::cout<<"Pixel total canvas: "<<_nbPixelsTotal<<std::endl;
        std::cout<<"Undo total canvas: "<<_nbPixelsTotal<<std::endl;
        std::cout<<"Pixel user(s): "<<_nbPixelsTotal<<std::endl;
        std::cout<<"Undo user(s): "<<_nbPixelsTotal<<std::endl;
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
    cv::Mat _outFrame;
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
        _outFrame.create(inputImg.size(),inputImg.type());
        if (canvas.timelapse())
        {
            _video.open("timelapse.mp4",cv::VideoWriter::fourcc('M','P','4','V'),10,_bgImg.size());
        }
        
    }

    ~drawing(){
        if (_video.isOpened())
        {
            _video.release();
        }
    }

    void drawImage(std::vector<pxlsData> *pxlsList, palette palette, canvas canvas){
        uint64_t newStart = canvas.timeBegin();
        uint64_t newStop = canvas.timeBegin()+canvas.captureDuration();
        while (newStart+canvas.frameDuration()<=newStop){
            newStart = drawFrame(pxlsList,palette,canvas,newStart);
            if(canvas.timelapse()){
                cv::Mat tempFrame(_bgImg.size(),_bgImg.type());
                maskOp(tempFrame);
            }
        }
        
        
    }

private:
    void maskOp(cv::Mat tempFrame){
        if(canvas.overlay()){
            alphaMerge(_bgImg,_fgImg,tempImg);
            if(canvas.heatmap()){
                alphaFunc(tempImg,tempImg,0.3);
                alphaMerge(tempImg,_heatImg,tempImg);
                _video.write(tempImg);
                alphaFunc(_heatImg,_heatImg,0.8);
            }else{
                _video.write(tempImg);
            }
        }else{
            _bgImg.copyTo(tempImg);
            if(canvas.heatmap()){
                alphaFunc(tempImg,tempImg,0.3);
                alphaMerge(tempImg,_heatImg,tempImg);
                _video.write(tempImg);
            }else{
                _video.write(tempImg);
            }
        }
    }

    void drawPxls(cv::Mat img, pxlsData pxlsData, palette palette){
        std::vector<int> currentCoord = pxlsData.getCoord();
        std::vector<uint> currentColor = palette.getColor(pxlsData.getColorIndex());
        cv::circle(img,cv::Point(currentCoord[0],currentCoord[1]),0,
        cv::Scalar(currentColor[0],currentColor[1],currentColor[2]),cv::FILLED);
    }

    void drawPxls(cv::Mat img, pxlsData pxlsData){
        std::vector<int> currentCoord = pxlsData.getCoord();
        std::vector<uint> currentColor = pxlsData.getHeatColor();
        cv::circle(img,cv::Point(currentCoord[0],currentCoord[1]),0,
        cv::Scalar(currentColor[0],currentColor[1],currentColor[2]),cv::FILLED);
    }

    uint64_t drawFrame(std::vector<pxlsData> *pxlsList, palette palette, canvas canvas, uint64_t tStart){
        int idxStart = lowerBoundIdx(pxlsList,tStart);
        int idxStop = lowerBoundIdx(pxlsList,tStart+canvas.frameDuration());
        for (int i = idxStart; i < idxStop; i++){   
            drawPxls(_fgImg,pxlsList->at(i),palette);
            if(canvas.heatmap()){
                drawPxls(_heatImg,pxlsList->at(i));
            }
        }
        return tStart+canvas.frameDuration();
    }
};
}

namespace hash{
    bool isMyPxls(const std::string digest,const std::string randomHash);
}