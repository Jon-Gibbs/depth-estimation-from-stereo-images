#include "../rectifier.h"
#include "rect_compare.h"

// PRE:  unrectified_images/left_frame_1.jpg and right_frame_1.jpg exist and are the same size
//       camera/intrinsics.yaml is a valid calibration file
// POST: three windows are displayed showing the unrectified pair, the custom-rectified pair,
//       and the OpenCV-rectified pair; pressing any key advances to epipolar line overlays
//       for each method in sequence
int main()
{
    const std::string leftImagePath = "../../unrectified_images/left_frame_1.jpg";
    const std::string rightImagePath = "../../unrectified_images/right_frame_1.jpg";
    const std::string calibrationPath = "../../camera/intrinsics.yaml";

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
        StereoRectifier customRectifier(calibrationPath, left_img.size());
        rect_compare opencvRectifier(calibrationPath, left_img.size());

        cv::Mat custom_L, custom_R;
        cv::Mat opencv_L, opencv_R;
        customRectifier.rectifyImages(left_img, right_img, custom_L, custom_R);
        opencvRectifier.rectifyImages(left_img, right_img, opencv_L, opencv_R);

        cv::Mat unrectified_pair, custom_pair, opencv_pair;
        cv::hconcat(left_img, right_img, unrectified_pair);
        cv::hconcat(custom_L, custom_R, custom_pair);
        cv::hconcat(opencv_L, opencv_R, opencv_pair);

        cv::imshow("Unrectified Pair", unrectified_pair);
        cv::imshow("Custom Rectification", custom_pair);
        cv::imshow("OpenCV Rectification", opencv_pair);
        cv::waitKey(0);

        // display epipolar line overlays for each method in sequence
        customRectifier.drawEpipolarLines(custom_L, custom_R);
        opencvRectifier.drawEpipolarLines(opencv_L, opencv_R);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Rectification failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}