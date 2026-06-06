#include "rectifier.h"
#include "disparity_mapper.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

int main()
{
    const std::string calibPath    = "../camera/camera_info.yaml";
    const cv::Size    imageSize(1280, 720);
    const int         numPairs = 6;

    StereoRectifier rectifier(calibPath, imageSize);
    DisparityMapper mapper(13);

    for (int i = 1; i <= numPairs; i++)
    {
        std::string leftPath  = "../unrectified_images/left_frame_"  + std::to_string(i) + ".jpg";
        std::string rightPath = "../unrectified_images/right_frame_" + std::to_string(i) + ".jpg";

        cv::Mat left  = cv::imread(leftPath);
        cv::Mat right = cv::imread(rightPath);

        if (left.empty() || right.empty())
        {
            std::cerr << "Failed to load pair " << i << "\n";
            continue;
        }

        cv::Mat rectLeft, rectRight;
        rectifier.rectifyImages(left, right, rectLeft, rectRight);

        std::string rectLeftPath  = "../rectified_images/left_rect_"  + std::to_string(i) + ".jpg";
        std::string rectRightPath = "../rectified_images/right_rect_" + std::to_string(i) + ".jpg";
        cv::imwrite(rectLeftPath,  rectLeft);
        cv::imwrite(rectRightPath, rectRight);
        
        rectLeft = cv::imread(rectLeftPath);
        rectRight = cv::imread(rectRightPath);
        cv::Mat disparity = mapper.computeDisparityMap(rectLeft, rectRight);
        cv::Mat depth = mapper.computeDepthMap(disparity);
        std::string depthPath = "../depth_maps/depth_" + std::to_string(i) + ".jpg";
        std::string dispPath = "../disparity_maps/disparity_" + std::to_string(i) + ".jpg";
        mapper.saveDepthImage(depth, depthPath, 6000.0f);
        mapper.saveDisparityImage(disparity, dispPath);


        std::cout << "Processed pair " << i << "\n";
    }

    return 0;
}
