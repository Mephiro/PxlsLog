#include "CanvasFunc.h"

namespace pxls{

void alphaMerge(cv::Mat src1, cv::Mat src2, cv::Mat dst){
    if ((src1.channels()==4) & (src2.channels()==4))
    {
        for (int i = 0; i < src1.rows; i++)
        {
            for (int j = 0; j < src1.cols; j++)
            {
                RGBA& rgbaDst = dst.ptr<RGBA>(i)[j];
                RGBA& rgbaSrc1 = src1.ptr<RGBA>(i)[j];
                RGBA& rgbaSrc2 = src2.ptr<RGBA>(i)[j];

                float dstAlpha = (1 - (1 - rgbaSrc2.alpha/255.f) * (1 - rgbaSrc1.alpha/255.f));
                float red =     ((rgbaSrc2.red/255.f)*(rgbaSrc2.alpha/255.f)/dstAlpha) +  ((rgbaSrc1.red/255.f)*(rgbaSrc1.alpha/255.f)*(1-rgbaSrc2.alpha/255.f)/dstAlpha);
                float green =   ((rgbaSrc2.green/255.f)*(rgbaSrc2.alpha/255.f)/dstAlpha) +  ((rgbaSrc1.green/255.f)*(rgbaSrc1.alpha/255.f)*(1-rgbaSrc2.alpha/255.f)/dstAlpha);
                float blue =    ((rgbaSrc2.blue/255.f)*(rgbaSrc2.alpha/255.f)/dstAlpha) +  ((rgbaSrc1.blue/255.f)*(rgbaSrc1.alpha/255.f)*(1-rgbaSrc2.alpha/255.f)/dstAlpha);
                
                rgbaDst.red = uchar(red*255);
                rgbaDst.green = uchar(green*255);
                rgbaDst.blue = uchar(blue*255);
                rgbaDst.alpha = uchar(dstAlpha*255);
            }
        }
    }else{
        std::cout << "Error : No alpha channel on one or both of the images" << std::endl;
    }   
}

void brigthnessFunc(cv::Mat src, cv::Mat dst, const float factor){
    std::vector<cv::Mat> channels(4);
    cv::split(src,channels);
    for (int i = 0; i < channels.size()-1; i++)
    {
        channels[i] = channels[i]*factor;
    }
    cv::merge(channels,dst);
}

void alphaFunc(cv::Mat src, cv::Mat dst, const float factor){
    std::vector<cv::Mat> channels(4);
    cv::split(src,channels);
    channels[3] = channels[3]*factor;
    cv::merge(channels,dst);
}

void pxlsHash(std::string userKeysFilename, std::string logFileFilename,std::vector<pxlsData> *pxlsList,canvas *canvas){
    std::fstream logFile(logFileFilename,std::ios::in);
    std::fstream userKeys(userKeysFilename,std::ios::in);
    std::string logLine;
    std::string userKey;
    std::string heatmapColor;
    while (std::getline(logFile,logLine)){
        pxlsData pxlsData(logLine,canvas);
        while (std::getline(userKeys,userKey)){
            if (userKey.find_first_of('#') == 0){
                heatmapColor = userKey;
            }else{
                std::string digest = pxlsData.pxlsDigest(userKey);
                if(hash::isMyPxls(digest,pxlsData.getRandomHash())){
                    pxlsData.addHeatColor(heatmapColor);
                    pxlsData.setMyPixel(true);
                }
            }
        }
        if(canvas->fullCanvas()){
            pxlsList->push_back(pxlsData);
        }else{
            if(pxlsData.getMyPixel()){pxlsList->push_back(pxlsData);}
        }
        pxlsData.countPxls(canvas);
        userKeys.clear();
        userKeys.seekg(0);
    }
    userKeys.close();
    logFile.close();
    std::cout<<"Extracting pxls done!"<<std::endl;
}

std::vector<uint> hexToBGR(char const *hexColor){
    std::vector<uint> color = {0,0,0};
    std::sscanf(hexColor,"#%02x%02x%02x",&color[2],&color[1],&color[0]);
    return color;
}

std::vector<std::string> lineParser(std::string line,char separator){
    int idx = 0;
    int idxFind = 0;
    std::vector<std::string> parse;
    while (idxFind >= 0)
    {
        idxFind = line.find(separator,idx);
        parse.push_back(line.substr(idx,idxFind-idx));
        idx = idxFind+1;
    }
    return parse;
}

uint64_t unixTimeConv(std::string strTime, std::string timezone){
    uint idx = strTime.find(',');
    std::string datetime = strTime.substr(0,idx);
    uint ms = std::stoi(strTime.substr(idx+1));
    struct std::tm tm;
    std::setlocale(LC_ALL,timezone.c_str());
    if(strptime(datetime.c_str(), "%Y-%m-%d %H:%M:%S",&tm) == NULL){
        return -1;
    }
    uint64_t unixTime = std::mktime(&tm)*1000 + ms;
    return unixTime;
}

int lowerBoundIdx(std::vector<pxlsData> *pxlsList, uint64_t unixTime){
    std::vector<pxlsData>::iterator itrStart = std::lower_bound(pxlsList->begin(),pxlsList->end(),unixTime);
    return itrStart - pxlsList->begin();
}

}

namespace hash{

bool isMyPxls(const std::string digest,const std::string randomHash){
    unsigned char hash[SHA256_DIGEST_LENGTH] = {0};

    char *msg = (char*)digest.c_str();
    unsigned int msgSize = strlen(msg);

    EVP_MD_CTX *sha256;
    sha256 = EVP_MD_CTX_new();
    EVP_MD_CTX_init(sha256);
    EVP_DigestInit_ex(sha256,EVP_sha256(),NULL);
    EVP_DigestUpdate(sha256,msg,msgSize);
    EVP_DigestFinal_ex(sha256,hash,&msgSize);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    std::string result = ss.str();
    return result==randomHash;
}
}