#include "rect_compare.h"

// PRE:  calibrationPath points to a valid YAML file with left/right intrinsics, distortion, R and T
//       imageSize matches the resolution the camera will capture at
// POST: remap tables are built via cv::stereoRectify and cv::initUndistortRectifyMap;
//       the object is ready to rectify image pairs
rect_compare::rect_compare(const std::string &calibrationPath, cv::Size imageSize)
{
    imageSize_ = imageSize;
    loadCalibration(calibrationPath);
    computeRectification();
    buildRemapTables();
}

// PRE:  path points to a valid YAML file containing all required calibration fields
// POST: K_L_, dist_L_, K_R_, dist_R_, R_, T_ are populated
void rect_compare::loadCalibration(const std::string &path)
{
    YAML::Node config = YAML::LoadFile(path);

    K_L_ = cv::Mat::zeros(3, 3, CV_64F);
    K_R_ = cv::Mat::zeros(3, 3, CV_64F);
    dist_L_ = cv::Mat::zeros(1, 5, CV_64F);
    dist_R_ = cv::Mat::zeros(1, 5, CV_64F);
    T_ = cv::Mat::zeros(3, 1, CV_64F);

    // left intrinsics
    K_L_.at<double>(0, 0) = config["left_camera"]["intrinsics"]["fx"].as<double>();
    K_L_.at<double>(1, 1) = config["left_camera"]["intrinsics"]["fy"].as<double>();
    K_L_.at<double>(0, 2) = config["left_camera"]["intrinsics"]["cx"].as<double>();
    K_L_.at<double>(1, 2) = config["left_camera"]["intrinsics"]["cy"].as<double>();
    K_L_.at<double>(2, 2) = 1.0;
    // right intrinsics
    K_R_.at<double>(0, 0) = config["right_camera"]["intrinsics"]["fx"].as<double>();
    K_R_.at<double>(1, 1) = config["right_camera"]["intrinsics"]["fy"].as<double>();
    K_R_.at<double>(0, 2) = config["right_camera"]["intrinsics"]["cx"].as<double>();
    K_R_.at<double>(1, 2) = config["right_camera"]["intrinsics"]["cy"].as<double>();
    K_R_.at<double>(2, 2) = 1.0;
    // left distortion
    dist_L_.at<double>(0, 0) = config["left_camera"]["disto"]["k1"].as<double>();
    dist_L_.at<double>(0, 1) = config["left_camera"]["disto"]["k2"].as<double>();
    dist_L_.at<double>(0, 2) = config["left_camera"]["disto"]["p1"].as<double>();
    dist_L_.at<double>(0, 3) = config["left_camera"]["disto"]["p2"].as<double>();
    dist_L_.at<double>(0, 4) = config["left_camera"]["disto"]["k3"].as<double>();
    // right distortion
    dist_R_.at<double>(0, 0) = config["right_camera"]["disto"]["k1"].as<double>();
    dist_R_.at<double>(0, 1) = config["right_camera"]["disto"]["k2"].as<double>();
    dist_R_.at<double>(0, 2) = config["right_camera"]["disto"]["p1"].as<double>();
    dist_R_.at<double>(0, 3) = config["right_camera"]["disto"]["p2"].as<double>();
    dist_R_.at<double>(0, 4) = config["right_camera"]["disto"]["k3"].as<double>();
    // rotation (Rodrigues vector -> 3x3 matrix)
    cv::Mat R_vec = cv::Mat::zeros(3, 1, CV_64F);
    R_vec.at<double>(0, 0) = config["rotation"]["x"].as<double>();
    R_vec.at<double>(1, 0) = config["rotation"]["y"].as<double>();
    R_vec.at<double>(2, 0) = config["rotation"]["z"].as<double>();
    cv::Rodrigues(R_vec, R_);
    // translation
    T_.at<double>(0, 0) = config["translation"]["x_pos"].as<double>();
    T_.at<double>(1, 0) = config["translation"]["y_pos"].as<double>();
    T_.at<double>(2, 0) = config["translation"]["z_pos"].as<double>();
}

// PRE:  K_L_, dist_L_, K_R_, dist_R_, R_, T_, imageSize_ are all populated
// POST: R_L_, R_R_, P_L_, P_R_, Q_ are computed via cv::stereoRectify
void rect_compare::computeRectification()
{
    cv::stereoRectify(K_L_, dist_L_, K_R_, dist_R_,
                      imageSize_, R_, T_,
                      R_L_, R_R_, P_L_, P_R_, Q_,
                      cv::CALIB_ZERO_DISPARITY, -1, imageSize_);
}

// PRE:  R_L_, R_R_, P_L_, P_R_, K_L_, K_R_, dist_L_, dist_R_, imageSize_ are all populated
// POST: map_L1_, map_L2_, map_R1_, map_R2_ are populated via cv::initUndistortRectifyMap
void rect_compare::buildRemapTables()
{
    cv::initUndistortRectifyMap(K_L_, dist_L_, R_L_, P_L_, imageSize_, CV_32FC1, map_L1_, map_L2_);
    cv::initUndistortRectifyMap(K_R_, dist_R_, R_R_, P_R_, imageSize_, CV_32FC1, map_R1_, map_R2_);
}

// PRE:  img_L and img_R are non-empty, same size as imageSize_, same type (e.g. CV_8UC3)
//       constructor has completed successfully (remap tables are built)
// POST: rectified_L and rectified_R contain the OpenCV-rectified image pair via cv::remap
//       corresponding features in rectified_L and rectified_R lie on the same horizontal scanline
void rect_compare::rectifyImages(const cv::Mat &img_L, const cv::Mat &img_R,
                                 cv::Mat &rectified_L, cv::Mat &rectified_R)
{
    if (img_L.empty() || img_R.empty())
        throw std::runtime_error("rectifyImages: input images must be non-empty");
    if (img_L.size() != imageSize_ || img_R.size() != imageSize_)
        throw std::runtime_error("rectifyImages: input image size does not match calibration image size");
    if (img_L.type() != img_R.type())
        throw std::runtime_error("rectifyImages: left and right image types must match");

    cv::remap(img_L, rectified_L, map_L1_, map_L2_, cv::INTER_LINEAR);
    cv::remap(img_R, rectified_R, map_R1_, map_R2_, cv::INTER_LINEAR);
}

// PRE:  rectified_L and rectified_R are non-empty and the same size
// POST: horizontal lines are drawn across both images at regular intervals
//       the combined image is displayed in a window for visual verification
void rect_compare::drawEpipolarLines(cv::Mat &rectified_L, cv::Mat &rectified_R)
{
    cv::Mat combined;
    cv::hconcat(rectified_L, rectified_R, combined);

    const int step = 30;
    const cv::Scalar lineColor(0, 255, 0);
    const int thickness = 1;

    for (int y = 0; y < combined.rows; y += step)
    {
        cv::line(combined,
                 cv::Point(0, y),
                 cv::Point(combined.cols - 1, y),
                 lineColor, thickness, cv::LINE_AA);
    }

    cv::imshow("OpenCV Rectification - Epipolar Lines", combined);
    cv::waitKey(0);
}
