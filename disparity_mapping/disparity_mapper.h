#pragma once
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include "template_matcher.h"

class DisparityMapper {
public:
    DisparityMapper(int windowSize, int stride = 1, int maxDisparity = 64);

    // Computes disparity map from a stereo image pair.
    // Returns a float Mat of raw disparity values.
    cv::Mat computeDisparityMap(const cv::Mat &img_L, const cv::Mat &img_R);

    // Normalizes the disparity map to an 8-bit grayscale image and saves it to disk.
    void saveDisparityImage(const cv::Mat &disparityMap, const std::string &outputPath);

    //precondition: disparity map 
    //postcondition: depth map
    cv::Mat computeDepthMap(const cv::Mat &disparityMap);

    //precondition: a valid file path to camera/camera_info.yaml
    //postcondition: baseline and focal length populated
    void loadCalibration(const std::string& filepath);
private:
    TemplateMatcher _matcher;
    int _windowSize;
    int _stride;
    int _maxDisparity;
    float baseline;
    float focal_length;
};