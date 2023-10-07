#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
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

std::vector<uint> hexToRGB(char const *hexColor);
std::vector<std::string> lineParser(std::string line,char separator);

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
    std::string _timezone = "CEST";

public:
    pxlsData(std::string inLine,std::string timezone){
        std::vector<std::string> parse;
        parse = lineParser(inLine,'\t');
        _date = parse[0];
        _unixDate = getUnixTime(parse[0],timezone);
        _randomHash = parse[1];
        _xy = {std::stoi(parse[2]),std::stoi(parse[3])};
        _colorIndex = std::stoi(parse[4]);
        _action = parse[5];
    }

    std::string pxlsDigest(std::string userKey){
        std::stringstream digest;
        digest<<_date<<','<<_xy[0]<<','<<_xy[1]<<','<<_colorIndex<<','<<userKey;
        return digest.str();
    }

    void addHeatColor(std::string heatColor){
        _heatmapColor = hexToRGB(heatColor.c_str());
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
    
private:
    uint64_t getUnixTime(std::string strTime, std::string zonetime){
        uint idx = strTime.find(',');
        std::string datetime = strTime.substr(0,idx);
        uint ms = std::stoi(strTime.substr(idx+1));
        struct std::tm tm;
        std::setlocale(LC_ALL,zonetime.c_str());
        if(strptime(datetime.c_str(), "%Y-%m-%d %H:%M:%S",&tm) == NULL){
            return -1;
        }
        uint64_t unixTime = std::mktime(&tm)*1000 + ms;
        return unixTime;
    }
};

void alphaMerge(cv::Mat src1, cv::Mat src2, cv::Mat dst);
void brigthnessFunc(cv::Mat src, cv::Mat dst, const float factor);
void alphaFunc(cv::Mat src, cv::Mat dst, const float factor);
void pxlsHash(std::string userKeysFilename, std::string logFileFilename,std::vector<pxlsData> *pxlsList,std::string timezone);
}

namespace hash{
    bool isMyPxls(const std::string digest,const std::string randomHash);
}