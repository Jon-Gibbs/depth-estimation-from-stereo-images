#include "disparity_mapper.h"
#include <iostream>
#include <opencv2/opencv.hpp>


DisparityMapper::DisparityMapper(int windowSize, int stride, int maxDisparity)
    : _matcher(windowSize, maxDisparity), _windowSize(windowSize), _stride(stride), _maxDisparity(maxDisparity) {
        loadCalibration("../camera/camera_info.yaml");
    }

cv::Mat DisparityMapper::computeDisparityMap(const cv::Mat &img_L, const cv::Mat &img_R) {
    int rows = img_L.rows;
    int cols = img_L.cols;
    int half = _windowSize / 2;

    // initialize disparity map to 0
    cv::Mat disparityMap = cv::Mat::zeros(rows, cols, CV_32F);

    int processed = 0;
    int total = ((rows - 2 * half) / _stride) * ((cols - 2 * half) / _stride);

    for (int y = half; y < rows - half; y += _stride) {
        for (int x = half; x < cols - half; x += _stride) {
            int x_R = 0;
            int y_R = 0;

            _matcher.findMatchingWindow(img_L, img_R, x, y, x_R, y_R);

            // disparity is the horizontal distance between matched pixels
            float disparity = std::abs(x - x_R);
            disparityMap.at<float>(y, x) = disparity;

            // fill in the stride block so the output isn't sparse
            if (_stride > 1) {
                for (int dy = 0; dy < _stride && y + dy < rows; dy++)
                    for (int dx = 0; dx < _stride && x + dx < cols; dx++)
                        disparityMap.at<float>(y + dy, x + dx) = disparity;
            }
        }

        // log progress
        processed += (cols - 2 * half) / _stride;
        int percent = (int)(100.0f * processed / total);
        std::cout << "\rComputing disparity map: " << percent << "%" << std::flush;
    }

    std::cout << "\rComputing disparity map: 100%" << std::endl;
    return disparityMap;
}
cv::Mat DisparityMapper::computeDepthMap(const cv::Mat& disparityMap){
    cv::Mat depthMap = cv::Mat::zeros(disparityMap.rows, disparityMap.cols, CV_32F);
    //loop through disparity map
    float disp =0;
    float depth= 0;
    float fxB = focal_length * baseline;
    for(int i=0; i<disparityMap.rows;i++){
        for(int j=0;j<disparityMap.cols; j++){
            disp = disparityMap.at<float>(i,j);
            if(disp > 0.0f){
            depth = fxB/disp;
            depthMap.at<float>(i,j) = depth;
            }
         }
    }
    return depthMap;
}

void DisparityMapper::saveDepthImage(const cv::Mat &depthMap, const std::string &outputPath, float maxDepth) {
    // clamp to [0, maxDepth] — removes outlier far pixels that crush the color range
    cv::Mat clamped;
    cv::threshold(depthMap, clamped, maxDepth, maxDepth, cv::THRESH_TRUNC);

    // normalize the clamped range to 0-255
    cv::Mat visual;
    cv::normalize(clamped, visual, 0, 255, cv::NORM_MINMAX);
    visual.convertTo(visual, CV_8U);

    cv::Mat colorMap;
    cv::applyColorMap(visual, colorMap, cv::COLORMAP_JET);

    cv::imwrite(outputPath, colorMap);
    std::cout << "Depth image saved to: " << outputPath << std::endl;
}

void DisparityMapper::saveDisparityImage(const cv::Mat &disparityMap, const std::string &outputPath) {
    // normalize to 0-255 range for visualization
    cv::Mat visual;
    cv::normalize(disparityMap, visual, 0, 255, cv::NORM_MINMAX);
    visual.convertTo(visual, CV_8U);

    // apply a colormap so depth differences are easier to see
    cv::Mat colorMap;
    cv::applyColorMap(visual, colorMap, cv::COLORMAP_JET);

    cv::imwrite(outputPath, colorMap);
    std::cout << "Disparity image saved to: " << outputPath << std::endl;
}

void DisparityMapper::loadCalibration(const std::string& filepath){
    YAML::Node config = YAML::LoadFile(filepath);
    //populate baseline
    baseline = config["translation"]["x_pos"].as<float>();
    //populate focal length
    focal_length = config["left_camera"]["intrinsics"]["fx"].as<float>();
}