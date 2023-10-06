#pragma once

#include <iostream>
#include <iomanip>
#include <openssl/evp.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define SHA256_DIGEST_LENGTH 32

namespace pxls{

void alphaMerge(cv::Mat src1, cv::Mat src2, cv::Mat dst);
void brigthnessFunc(cv::Mat src, cv::Mat dst, const float factor);
void alphaFunc(cv::Mat src, cv::Mat dst, const float factor);
}

namespace hash{
    bool SHAdigestComp(const std::string digest,const std::string randomHash);
}