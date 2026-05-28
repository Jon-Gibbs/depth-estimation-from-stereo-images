#include "rectifier.h"

// PRE:  calibrationPath points to a valid YAML file with left/right intrinsics, distortion, R and T
//       imageSize matches the resolution the camera will capture at
// POST: remap tables are built and the object is ready to rectify image pairs
StereoRectifier::StereoRectifier(const std::string &calibrationPath, cv::Size imageSize)
{

    try
    {
        YAML::Node config = YAML::LoadFile(calibrationPath);
    }
    catch (const YAML::BadFile &e)
    {
        std::cerr << "Error: could not open file." << std::endl;
    }
    // inititalize intrinsics matricies
    K_L_ = cv::Mat::zeros(3, 3, CV_32F);
    K_R_ = cv::Mat::zeros(3, 3, CV_32F);
    // intialize distortion matricies
    dist_L_ = cv::Mat<cv::Mat<float>>::zeroes(1, 5, CV_32F);
    dist_R_ = cv::Mat<cv::Mat<float>>::zeroes(1, 5, CV_32F);
    // initaliize transaltion and rotation
    R_ = cv::Mat<cv::Mat<float>>::zeroes(1, 2, CV_32F);
    T_ = cv::Mat<cv::Mat<float>>::zeroes(1, 2, CV_32F);
    //
    // populate matricies
    loadCalibration(calibrationPath);
    this->imageSize_ = imageSize;
}
// PRE:  path points to a valid YAML file containing all required calibration fields
// POST: K_L_, dist_L_, K_R_, dist_R_, R_, T_ are populated
void StereoRectifier::loadCalibration(const std::string &path)
{
    // populate intrinsics matricies
    // left camera
    K_L_[0][0] = config["left_camera"]["intrinsics"]["fx"].as<float>();
    K_L_[1][1] = config["left_camera"]["intrinsics"]["fy"].as<float>();
    K_L_[0][2] = config["left_camera"]["intrinsics"]["cx"].as<float>();
    K_L_[1][2] = config["left_camera"]["intrinsics"]["fy"].as<float>();
    K_L_[2][2] = 1;
    // right camera
    K_R_[0][0] = config["right_camera"]["intrinsics"]["fx"].as<float>();
    K_R_[1][1] = config["right_camera"]["intrinsics"]["fy"].as<float>();
    K_R_[0][2] = config["right_camera"]["intrinsics"]["cx"].as<float>();
    K_R_[1][2] = config["right_camera"]["intrinsics"]["fy"].as<float>();
    K_R_[2][2] = 1;
    // populate distortion matricies
    // left camera
    dist_L_[0][0] = config["left_camera"]["disto"]["k1"].as<float>();
    dist_L_[0][1] = config["left_camera"]["disto"]["k2"].as<float>();
    dist_L_[0][2] = config["left_camera"]["disto"]["p1"].as<float>();
    dist_L_[0][3] = config["left_camera"]["disto"]["p2"].as<float>();
    dist_L_[0][4] = config["left_camera"]["disto"]["k3"].as<float>();
    // right camera
    dist_R_[0][0] = config["left_camera"]["disto"]["k1"].as<float>();
    dist_R_[0][1] = config["left_camera"]["disto"]["k2"].as<float>();
    dist_R_[0][2] = config["left_camera"]["disto"]["p1"].as<float>();
    dist_R_[0][3] = config["left_camera"]["disto"]["p2"].as<float>();
    dist_R_[0][4] = config["left_camera"]["disto"]["k3"].as<float>();
    // populate rotation and tranlsation matricies
    // rotation
    R_[0][0] = config["translation"]["x"].as<float>();
    R_[0][1] = config["translation"]["y"].as<float>();
    R_[0][2] = config["translation"]["z"].as<float>();
    // translation
    T_[0][0] = config["translation"]["x"].as<float>();
    T_[0][1] = config["translation"]["y"].as<float>();
    T_[0][2] = config["translation"]["z"].as<float>();
}
// PRE:  K_L_, dist_L_, K_R_, dist_R_, R_, T_, imageSize_ are all populated
// POST: R_L_, R_R_, P_L_, P_R_, Q_ are populated
void StereoRectifier::computeRectification()
{
    // make a
}
// PRE:  R_L_, R_R_, P_L_, P_R_, K_L_, K_R_, dist_L_, dist_R_, imageSize_ are all populated
// POST: map_L1_, map_L2_, map_R1_, map_R2_ are populated and ready for use
void StereoRectifier::buildRemapTables()
{
}
// PRE:  img_L and img_R are non-empty, same size as imageSize_, same type (e.g. CV_8UC3)
//       constructor has completed successfully (remap tables are built)
// POST: rectified_L and rectified_R contain the rectified image pair
//       corresponding features in rectified_L and rectified_R lie on the same horizontal scanline
void StereoRectifier::rectifyImages(const cv::Mat &img_L, const cv::Mat &img_R,
                                    cv::Mat &rectified_L, cv::Mat &rectified_R)
{
}
// PRE:  rectified_L and rectified_R are non-empty and the same size
// POST: horizontal lines are drawn across both images at regular intervals
//       the combined image is displayed in a window for visual verification
void StereoRectifier::drawEpipolarLines(cv::Mat &rectified_L, cv::Mat &rectified_R)
{
}