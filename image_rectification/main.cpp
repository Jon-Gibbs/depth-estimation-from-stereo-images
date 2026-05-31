#include "rectifier.h"

int main()
{
    const std::string leftImagePath = "../unrectified_images/left_frame_1.jpg";
    const std::string rightImagePath = "../unrectified_images/right_frame_1.jpg";
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
         // 2. Define configuration parameters
    int minDisparity = 0;
    int numDisparities = 16 * 5; // Must be divisible by 16
    int blockSize = 5;           // Must be an odd number >= 1

    // 3. Create the StereoSGBM object
    cv::Ptr<cv::StereoSGBM> stereo = cv::StereoSGBM::create(
        minDisparity,
        numDisparities,
        blockSize,
        8 * 3 * blockSize * blockSize,   // P1: Smoothness penalty 1
        32 * 3 * blockSize * blockSize,  // P2: Smoothness penalty 2
        1,                               // disp12MaxDiff
        63,                              // preFilterCap
        10,                              // uniquenessRatio
        100,                             // speckleWindowSize
        32                               // speckleRange
    );

    // 4. Compute the raw disparity map
    cv::Mat disparityRaw;
    stereo->compute(imgLeft, imgRight, disparityRaw);

    // 5. Convert raw disparity for visualization
    // Stereo matches return 16-bit signed integers (CV_16S) scaled by 16
    cv::Mat disparityVisual;
    disparityRaw.convertTo(disparityVisual, CV_8U, 255.0 / (numDisparities * 16.0));

    // 6. Apply a colormap for better visibility
    cv::Mat disparityColored;
    cv::applyColorMap(disparityVisual, disparityColored, cv::COLORMAP_JET);

    // 7. Display the result
    cv::imshow("Left Image", imgLeft);
    cv::imshow("Disparity Map (Colored)", disparityColored);
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