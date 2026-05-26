#include <filestream>
#include <opencv2/opencv.hpp>
int main()
{

    // read a test file from unrectified_images
    cv::Mat left_img = cv::imread("../unrectified_images/left_frame_1.jpg", cv::IMREAD_COLOR);
    cv::Mat right_img = cv::imread("../unrectified_images/left_frame_2.jpg", cv::IMREAD_COLOR);
    // read your camera intrinsics from ../camera/intrinsics.yaml

    // create a rectifier
    // feed in your images and intrinsics and receive a rectified image
    // display those rectified images using opencv
    // write those rectified images to ../rectified_images
};