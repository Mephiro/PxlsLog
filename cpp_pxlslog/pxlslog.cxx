#include "CanvasFunc.h"

int main(){

std::cout << "OpenCV discovery" << std::endl;

cv::Mat in1Img,in2Img,dstImg;
in1Img = cv::imread(cv::samples::findFile("canvas-72-initial.png"),cv::IMREAD_UNCHANGED);

pxls::brigthnessFunc(in1Img,in1Img,0.3);
std::cout << in1Img.type() << std::endl;
std::cout << in1Img.type() << std::endl;

in2Img.create(in1Img.size(),in1Img.type());
dstImg.create(in1Img.size(),in1Img.type());

cv::cvtColor(in2Img,in2Img,cv::COLOR_RGB2RGBA);
cv::cvtColor(dstImg,dstImg,cv::COLOR_RGB2RGBA);
cv::circle(in2Img,cv::Point(200,200),50,cv::Scalar(0,0,255,100),cv::FILLED);
cv::circle(in2Img,cv::Point(200,270),70,cv::Scalar(0,0,255,170),cv::FILLED);
pxls::alphaMerge(in1Img,in2Img,dstImg);
cv::imwrite("output.png",dstImg);
cv::imwrite("in1.png",in1Img);
cv::imwrite("in2.png",in2Img);

bool result = hash::SHAdigestComp("hello world!","7509e5bda0c762d2bac7f90d758b5b2263fa01ccbc542ab5e3df163be08e6ca9");
std::cout << result << std::endl;
return 0;
}
