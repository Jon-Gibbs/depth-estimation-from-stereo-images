#include "rectifier.h"

int main()
{
    const std::string leftImagePath = "../unrectified_images/left_frame_1.jpg";
    const std::string rightImagePath = "../unrectified_images/left_frame_2.jpg";
    const std::string calibrationPath = "../camera/intrinsics.yaml";

    cv::Mat left_img = cv::imread(leftImagePath, cv::IMREAD_COLOR);
    cv::Mat right_img = cv::imread(rightImagePath, cv::IMREAD_COLOR);

    if (left_img.empty() || right_img.empty())
    {
        std::cerr << "Failed to load input stereo images." << std::endl;
        return 1;
    }
    if (left_img.size() != right_img.size())
    {
        std::cerr << "Input image sizes do not match." << std::endl;
        return 1;
    }

    try
    {
        StereoRectifier rectifier(calibrationPath, left_img.size());

        cv::Mat rectified_L;
        cv::Mat rectified_R;
        rectifier.rectifyImages(left_img, right_img, rectified_L, rectified_R);

        cv::Mat unrectified_pair;
        cv::Mat rectified_pair;
        cv::hconcat(left_img, right_img, unrectified_pair);
        cv::hconcat(rectified_L, rectified_R, rectified_pair);

        cv::imshow("Unrectified Pair", unrectified_pair);
        cv::imshow("Rectified Pair", rectified_pair);

        rectifier.drawEpipolarLines(rectified_L, rectified_R);

        cv::imwrite("../rectified_images/left_rectified.jpg", rectified_L);
        cv::imwrite("../rectified_images/right_rectified.jpg", rectified_R);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Rectification failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}