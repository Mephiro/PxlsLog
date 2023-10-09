#include "CanvasFunc.h"

namespace pxls{

std::mutex mtx;

void UI_init(){
    std::cout<<"┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓"<<"\n";
    std::cout<<"┃ ██████╗ ██╗  ██╗██╗     ███████╗██╗      ██████╗  ██████╗  ┃"<<"\n";
    std::cout<<"┃ ██╔══██╗╚██╗██╔╝██║     ██╔════╝██║     ██╔═══██╗██╔════╝  ┃"<<"\n";
    std::cout<<"┃ ██████╔╝ ╚███╔╝ ██║     ███████╗██║     ██║   ██║██║  ███╗ ┃"<<"\n";
    std::cout<<"┃ ██╔═══╝  ██╔██╗ ██║     ╚════██║██║     ██║   ██║██║   ██║ ┃"<<"\n";
    std::cout<<"┃ ██║     ██╔╝ ██╗███████╗███████║███████╗╚██████╔╝╚██████╔╝ ┃"<<"\n";
    std::cout<<"┃ ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝  ╚═════╝  ┃"<<"\n";
    std::cout<<"┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛"<<"\n";
    std::cout<<"\n\n";
    std::cout<<"Enter the crop area for timelapse (example : X0 Y0 X0+Xoff Y0+Yoff): ";
    uint cropX, cropY, cropXoff, cropYoff;
    std::cin >> cropX >> cropY >> cropXoff >> cropYoff;
    std::vector<uint> cropArea = {cropX,cropY,cropXoff,cropYoff};
}

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
        std::cout << "Error : No alpha channel on one or both of the images\n";
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

void threadingHash(int nbCore, std::vector<pxlsData> *PxlsList, const std::string pxlsLogName,
                   const std::string keysLogName, canvas *canvas){

    std::cout<<"Extracting pxls in progress...\n";

    std::fstream userPxls(pxlsLogName,std::ios::in);
    int lineCount = 0;
    std::string line;
    while (std::getline(userPxls,line)){
        lineCount++;
    }
    userPxls.close();

    int increment = lineCount/nbCore;

    std::fstream userKeys(keysLogName,std::ios::in);
    std::vector<std::string> userKeyVec;
    std::string userKey;
    while (std::getline(userKeys,userKey)){
        userKeyVec.push_back(userKey);
    }
    userKeys.close();

    std::vector<std::thread> vecThread;
    for (int core = 0; core < nbCore; core++){
        if (core == nbCore-1){
            vecThread.push_back(std::thread(pxlsHash,PxlsList,pxlsLogName,userKeyVec,canvas,core*increment,lineCount));
        }else{
            vecThread.push_back(std::thread(pxlsHash,PxlsList,pxlsLogName,userKeyVec,canvas,core*increment,(core+1)*increment));
        }
    }
    for(int core = 0; core < nbCore; core++){
        vecThread.at(core).join();
    }
    std::sort(PxlsList->begin(),PxlsList->end());
    std::cout<<"Extracting pxls done!\n";
}

void pxlsHash(std::vector<pxlsData> *pxlsList, std::string pxlsLogName, std::vector<std::string> userKeys,
              canvas *canvas, int lineStart, int lineStop){
    
    int lineCount = 0;
    std::fstream logFile(pxlsLogName,std::ios::in);
    std::string logLine;
    std::string userKey;
    std::string heatmapColor = "#0000FF";
    while (std::getline(logFile,logLine)){
        if(lineCount>=lineStart && lineCount<lineStop){
            pxlsData pxlsData(logLine,canvas);
            heatmapColor = "#0000FF";
            for (int i = 0; i < userKeys.size(); i++){
                userKey = userKeys.at(i);
                if (userKey.find_first_of('#') == 0){
                    heatmapColor = userKey;
                }else{
                    std::string digest = pxlsData.pxlsDigest(userKey);
                    if(hash::isMyPxls(digest,pxlsData.getRandomHash())){
                        pxlsData.addHeatColor(heatmapColor);
                        pxlsData.setMyPixel(true);
                        i = userKeys.size();
                    }
                }
            }
            mtx.lock();
            if(canvas->fullCanvas()){
                pxlsList->push_back(pxlsData);
            }else{
                if(pxlsData.getMyPixel()){pxlsList->push_back(pxlsData);}
            }
            pxlsData.countPxls(canvas);
            mtx.unlock();
        }
        lineCount++;
    }
    logFile.close();
}

std::vector<uint> hexToBGR(char const *hexColor){
    std::vector<uint> color = {0,0,0};
    std::sscanf(hexColor,"#%02x%02x%02x",&color[0],&color[1],&color[2]);
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

std::vector<std::string>  subVector(const std::vector<std::string> *vec, size_t startIdx, size_t endIdx){
    if (startIdx >= vec->size() || endIdx > vec->size() || startIdx > endIdx) {
        throw std::out_of_range("Invalid subvector indices");
    }
    
    auto start = vec->begin() + startIdx;
    auto end = vec->begin() + endIdx;

    return std::vector<std::string>(start, end);
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