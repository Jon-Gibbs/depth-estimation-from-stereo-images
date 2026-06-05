#pragma once

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>

class rect_compare
{
public:
    // PRE:  calibrationPath points to a valid YAML file with left/right intrinsics, distortion, R and T
    //       imageSize matches the resolution the camera will capture at
    // POST: remap tables are built via cv::stereoRectify and cv::initUndistortRectifyMap;
    //       the object is ready to rectify image pairs
    rect_compare(const std::string &calibrationPath, cv::Size imageSize);

    // PRE:  img_L and img_R are non-empty, same size as imageSize_, same type (e.g. CV_8UC3)
    //       constructor has completed successfully (remap tables are built)
    // POST: rectified_L and rectified_R contain the OpenCV-rectified image pair via cv::remap
    //       corresponding features in rectified_L and rectified_R lie on the same horizontal scanline
    void rectifyImages(const cv::Mat &img_L, const cv::Mat &img_R,
                       cv::Mat &rectified_L, cv::Mat &rectified_R);

    // PRE:  rectified_L and rectified_R are non-empty and the same size
    // POST: horizontal lines are drawn across both images at regular intervals
    //       the combined image is displayed in a window for visual verification
    void drawEpipolarLines(cv::Mat &rectified_L, cv::Mat &rectified_R);

private:
    // PRE:  path points to a valid YAML file containing all required calibration fields
    // POST: K_L_, dist_L_, K_R_, dist_R_, R_, T_ are populated
    void loadCalibration(const std::string &path);

    // PRE:  K_L_, dist_L_, K_R_, dist_R_, R_, T_, imageSize_ are all populated
    // POST: R_L_, R_R_, P_L_, P_R_, Q_ are computed via cv::stereoRectify
    void computeRectification();

    // PRE:  R_L_, R_R_, P_L_, P_R_, K_L_, K_R_, dist_L_, dist_R_, imageSize_ are all populated
    // POST: map_L1_, map_L2_, map_R1_, map_R2_ are populated via cv::initUndistortRectifyMap
    void buildRemapTables();

    // calibration parameters
    cv::Mat K_L_, dist_L_;
    cv::Mat K_R_, dist_R_;
    cv::Mat R_, T_;

    // rectification transforms
    cv::Mat R_L_, R_R_;
    cv::Mat P_L_, P_R_;
    cv::Mat Q_;

    // remap tables
    cv::Mat map_L1_, map_L2_;
    cv::Mat map_R1_, map_R2_;

    cv::Size imageSize_;
};