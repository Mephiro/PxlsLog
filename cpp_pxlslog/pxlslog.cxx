#include "CanvasFunc.h"

int main(){

std::cout << "OpenCV discovery" << std::endl;

cv::Mat in1Img,in2Img,dstImg;
in1Img = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);

pxls::brigthnessFunc(in1Img,in1Img,0.3);

in2Img.create(in1Img.size(),in1Img.type());
dstImg.create(in1Img.size(),in1Img.type());
cv::cvtColor(in2Img,in2Img,cv::COLOR_RGB2RGBA);
cv::cvtColor(dstImg,dstImg,cv::COLOR_RGB2RGBA);

std::string userKeysName = "./Mephi.txt";
std::string logFileName = "./pixels_c72.sanit.log";
std::string paletteFileName = "./palette_c73.txt";
std::vector<pxls::pxlsData> pxlsList;
pxls::palette palette(paletteFileName);

pxls::pxlsHash(userKeysName,logFileName,&pxlsList,"CEST");
std::cout<<"Extracting pxls done!"<<std::endl;

for (int i = 0; i < pxlsList.size(); i++)
{
    std::vector<int> coord = pxlsList[i].getCoord();
    std::vector<uint> color = palette.getColor(pxlsList[i].getColorIndex());
    cv::circle(in2Img,cv::Point(coord[0],coord[1]),0,cv::Scalar(color[0],color[1],color[2],255),cv::FILLED);

}
std::cout<<"Drawing pxls done!"<<std::endl;

pxls::alphaMerge(in1Img,in2Img,dstImg);
cv::imwrite("output.png",dstImg);
cv::imwrite("in1.png",in1Img);
cv::imwrite("in2.png",in2Img);

return 0;
}
