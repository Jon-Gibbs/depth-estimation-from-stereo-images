#include "disparity_mapper.h"
#include <opencv2/opencv.hpp>


int main() {
    const std::string leftImagePath = "../rectified_images/left_rect_1.jpg";
    const std::string rightImagePath = "../rectified_images/right_rect_2.jpg";

    // take in rectified images
    cv::Mat left_img = cv::imread(leftImagePath, cv::IMREAD_COLOR);
    cv::Mat right_img = cv::imread(rightImagePath, cv::IMREAD_COLOR);

    //TemplateMatcher tm(9);
    DisparityMapper dm(9);
    cv::Mat output = dm.computeDisparityMap(left_img, right_img);
    cv::Mat depth_map = dm.computeDepthMap(output);
    dm.saveDisparityImage(depth_map, "../depth_maps/depth_map.jpg");
    dm.saveDisparityImage(output, "../disparity_maps/disparity_map.jpg");

    /*int xValue, yValue;
    tm.findMatchingWindow(left_img, right_img, 500, 500, xValue, yValue);
    cv::Mat left_output = left_img.clone();
    cv::Mat right_output = right_img.clone();

    tm.drawMatchingPoints(left_img, right_img, 500, 500, xValue, yValue, left_output, right_output);
    cv::imwrite("C:/Users/cooky/Documents/depth-estimation-from-stereo-images/other_image_output/left_output.jpg", left_output);
    cv::imwrite("C:/Users/cooky/Documents/depth-estimation-from-stereo-images/other_image_output/right_output.jpg", right_output);*/
    return 0;
}