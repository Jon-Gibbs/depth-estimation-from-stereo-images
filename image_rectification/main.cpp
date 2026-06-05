#include "rectifier.h"

int main()
{
    const std::string calibrationPath = "../camera/camera_info.yaml";

    for (int i = 1; i <= 6; i++)
    {
        std::string leftImagePath  = "../unrectified_images/left_frame_"  + std::to_string(i) + ".jpg";
        std::string rightImagePath = "../unrectified_images/right_frame_" + std::to_string(i) + ".jpg";

        cv::Mat left_img  = cv::imread(leftImagePath,  cv::IMREAD_COLOR);
        cv::Mat right_img = cv::imread(rightImagePath, cv::IMREAD_COLOR);

        if (left_img.empty() || right_img.empty())
        {
            std::cerr << "Failed to load pair " << i << std::endl;
            continue;
        }
        if (left_img.size() != right_img.size())
        {
            std::cerr << "Image sizes do not match for pair " << i << std::endl;
            continue;
        }

        try
        {
            StereoRectifier rectifier(calibrationPath, left_img.size());

            cv::Mat rectified_L, rectified_R;
            rectifier.rectifyImages(left_img, right_img, rectified_L, rectified_R);

            cv::imwrite("../rectified_images/left_rect_"  + std::to_string(i) + ".jpg", rectified_L);
            cv::imwrite("../rectified_images/right_rect_" + std::to_string(i) + ".jpg", rectified_R);

            std::cout << "Rectified pair " << i << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "Rectification failed for pair " << i << ": " << e.what() << std::endl;
        }
    }

    return 0;
}