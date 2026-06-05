#pragma once
#include <opencv2/opencv.hpp>
#include "template_matcher.h"

class DisparityMapper {
public:
    DisparityMapper(int windowSize, int stride = 1);

    // Computes disparity map from a stereo image pair.
    // Returns a float Mat of raw disparity values.
    cv::Mat computeDisparityMap(const cv::Mat &img_L, const cv::Mat &img_R);

    // Normalizes the disparity map to an 8-bit grayscale image and saves it to disk.
    void saveDisparityImage(const cv::Mat &disparityMap, const std::string &outputPath);

private:
    TemplateMatcher _matcher;
    int _windowSize;
    int _stride;
};