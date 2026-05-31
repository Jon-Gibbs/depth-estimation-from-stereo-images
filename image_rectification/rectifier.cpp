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
    dist_L_ = cv::Mat::zeros(1, 5, CV_32F);
    dist_R_ = cv::Mat::zeros(1, 5, CV_32F);
    // initaliize transaltion and rotation
    R_ = cv::Mat::zeros(3, 1, CV_32F);
    T_ = cv::Mat::zeros(3, 1, CV_32F);
    //
    // populate matricies
    loadCalibration(calibrationPath);
    this->imageSize_ = imageSize;

    // Build all rectification transforms and remap tables once at startup.
    computeRectification();
    buildRemapTables();
}
// PRE:  path points to a valid YAML file containing all required calibration fields
// POST: K_L_, dist_L_, K_R_, dist_R_, R_, T_ are populated
void StereoRectifier::loadCalibration(const std::string &path)
{
    YAML::Node config = YAML::LoadFile(path);
    // populate intrinsics matricies
    // left camera
    K_L_.at<float>(0, 0) = config["left_camera"]["intrinsics"]["fx"].as<float>();
    K_L_.at<float>(1, 1) = config["left_camera"]["intrinsics"]["fy"].as<float>();
    K_L_.at<float>(0, 2) = config["left_camera"]["intrinsics"]["cx"].as<float>();
    K_L_.at<float>(1, 2) = config["left_camera"]["intrinsics"]["cy"].as<float>();
    K_L_.at<float>(2, 2) = 1;
    // right camera
    K_R_.at<float>(0, 0) = config["right_camera"]["intrinsics"]["fx"].as<float>();
    K_R_.at<float>(1, 1) = config["right_camera"]["intrinsics"]["fy"].as<float>();
    K_R_.at<float>(0, 2) = config["right_camera"]["intrinsics"]["cx"].as<float>();
    K_R_.at<float>(1, 2) = config["right_camera"]["intrinsics"]["cy"].as<float>();
    K_R_.at<float>(2, 2) = 1;
    // populate distortion matricies
    // left camera
    dist_L_.at<float>(0, 0) = config["left_camera"]["disto"]["k1"].as<float>();
    dist_L_.at<float>(0, 1) = config["left_camera"]["disto"]["k2"].as<float>();
    dist_L_.at<float>(0, 2) = config["left_camera"]["disto"]["p1"].as<float>();
    dist_L_.at<float>(0, 3) = config["left_camera"]["disto"]["p2"].as<float>();
    dist_L_.at<float>(0, 4) = config["left_camera"]["disto"]["k3"].as<float>();
    // right camera
    dist_R_.at<float>(0, 0) = config["right_camera"]["disto"]["k1"].as<float>();
    dist_R_.at<float>(0, 1) = config["right_camera"]["disto"]["k2"].as<float>();
    dist_R_.at<float>(0, 2) = config["right_camera"]["disto"]["p1"].as<float>();
    dist_R_.at<float>(0, 3) = config["right_camera"]["disto"]["p2"].as<float>();
    dist_R_.at<float>(0, 4) = config["right_camera"]["disto"]["k3"].as<float>();
    // populate rotation and tranlsation matricies
    // rotation of the right camera relative to the left camera
    // load euler angles into a temporary vector
    cv::Mat R_vec = cv::Mat::zeros(3, 1, CV_32F);
    R_vec.at<float>(0, 0) = config["rotation"]["x"].as<float>();
    R_vec.at<float>(1, 0) = config["rotation"]["y"].as<float>();
    R_vec.at<float>(2, 0) = config["rotation"]["z"].as<float>();

    // convert to 3x3 rotation matrix
    cv::Rodrigues(R_vec, R_);
    // translation of the right camera relative to the left camera
    T_.at<float>(0, 0) = config["translation"]["x_pos"].as<float>();
    T_.at<float>(1, 0) = config["translation"]["y_pos"].as<float>();
    T_.at<float>(2, 0) = config["translation"]["z_pos"].as<float>();
}
// PRE:  K_L_, dist_L_, K_R_, dist_R_, R_, T_, imageSize_ are all populated
// POST: R_L_, R_R_, P_L_, P_R_, Q_ are populated
void StereoRectifier::computeRectification()
{
    // compute R_L and R_R
    // e1: along baseline, always pointing in +X so the rectified frame is right-side-up
    cv::Mat e1 = T_ / cv::norm(T_);
    if (e1.at<float>(0) < 0) e1 = -e1;

    // e2: new Y axis (downward) = z × e1
    cv::Mat z = (cv::Mat_<float>(3, 1) << 0.0f, 0.0f, 1.0f);
    cv::Mat e2 = z.cross(e1);
    e2 = e2 / cv::norm(e2);

    // e3: new Z axis (forward) = e1 × e2 (right-hand rule, guaranteed positive Z)
    cv::Mat e3 = e1.cross(e2);

    // stack e1, e2, e3 into rotation matrix (each as a row)
    cv::Mat R_rect = (cv::Mat_<float>(3, 3) << e1.at<float>(0),
                      e1.at<float>(1), e1.at<float>(2),
                      e2.at<float>(0), e2.at<float>(1), e2.at<float>(2),
                      e3.at<float>(0), e3.at<float>(1), e3.at<float>(2));

    // compute each camera's rotation
    R_L_ = R_rect;      // left is reference, R_L_original = identity
    R_R_ = R_rect * R_; // right includes the extrinsic rotation

    // compute P_L and P_R
    float fx = K_L_.at<float>(0, 0);
    float fy = K_L_.at<float>(1, 1);
    float cx = K_L_.at<float>(0, 2);
    float cy = K_L_.at<float>(1, 2);
    float tx = T_.at<float>(0); // baseline translation

    P_L_ = (cv::Mat_<float>(3, 4) << fx, 0, cx, 0,
            0, fy, cy, 0,
            0, 0, 1, 0);
    P_R_ = (cv::Mat_<float>(3, 4) << fx, 0, cx, fx * tx,
            0, fy, cy, 0,
            0, 0, 1, 0);
    // compute Q
    Q_ = (cv::Mat_<float>(4, 4) << 1, 0, 0, -cx,
          0, 1, 0, -cy,
          0, 0, 0, fx,
          0, 0, -1.0f / T_.at<float>(0), (cx - K_R_.at<float>(0, 2)) / T_.at<float>(0));
}
// PRE:  R_L_, R_R_, P_L_, P_R_, K_L_, K_R_, dist_L_, dist_R_, imageSize_ are all populated
// POST: map_L1_, map_L2_, map_R1_, map_R2_ are populated and ready for use
void StereoRectifier::buildRemapTables()
{
    auto buildSingleMap = [&](const cv::Mat &K, const cv::Mat &dist, const cv::Mat &R_rect,
                              const cv::Mat &P_rect, cv::Mat &map1, cv::Mat &map2)
    {
        map1.create(imageSize_, CV_32FC1);
        map2.create(imageSize_, CV_32FC1);

        const float fx = K.at<float>(0, 0);
        const float fy = K.at<float>(1, 1);
        const float cx = K.at<float>(0, 2);
        const float cy = K.at<float>(1, 2);

        const float k1 = dist.at<float>(0, 0);
        const float k2 = dist.at<float>(0, 1);
        const float p1 = dist.at<float>(0, 2);
        const float p2 = dist.at<float>(0, 3);
        const float k3 = dist.at<float>(0, 4);

        const float fxp = P_rect.at<float>(0, 0);
        const float fyp = P_rect.at<float>(1, 1);
        const float cxp = P_rect.at<float>(0, 2);
        const float cyp = P_rect.at<float>(1, 2);

        cv::Mat R_inv = R_rect.t();

        for (int v = 0; v < imageSize_.height; ++v)
        {
            for (int u = 0; u < imageSize_.width; ++u)
            {
                // Convert rectified pixel to a normalized rectified camera ray.
                const float x_rect = (static_cast<float>(u) - cxp) / fxp;
                const float y_rect = (static_cast<float>(v) - cyp) / fyp;

                cv::Mat ray_rect = (cv::Mat_<float>(3, 1) << x_rect, y_rect, 1.0f);
                cv::Mat ray_orig = R_inv * ray_rect;

                const float inv_z = 1.0f / ray_orig.at<float>(2, 0);
                const float x = ray_orig.at<float>(0, 0) * inv_z;
                const float y = ray_orig.at<float>(1, 0) * inv_z;

                const float r2 = x * x + y * y;
                const float r4 = r2 * r2;
                const float r6 = r4 * r2;
                const float radial = 1.0f + k1 * r2 + k2 * r4 + k3 * r6;

                const float x_dist = x * radial + 2.0f * p1 * x * y + p2 * (r2 + 2.0f * x * x);
                const float y_dist = y * radial + p1 * (r2 + 2.0f * y * y) + 2.0f * p2 * x * y;

                map1.at<float>(v, u) = fx * x_dist + cx;
                map2.at<float>(v, u) = fy * y_dist + cy;
            }
        }
    };

    buildSingleMap(K_L_, dist_L_, R_L_, P_L_, map_L1_, map_L2_);
    buildSingleMap(K_R_, dist_R_, R_R_, P_R_, map_R1_, map_R2_);
}
// PRE:  img_L and img_R are non-empty, same size as imageSize_, same type (e.g. CV_8UC3)
//       constructor has completed successfully (remap tables are built)
// POST: rectified_L and rectified_R contain the rectified image pair
//       corresponding features in rectified_L and rectified_R lie on the same horizontal scanline
void StereoRectifier::rectifyImages(const cv::Mat &img_L, const cv::Mat &img_R,
                                    cv::Mat &rectified_L, cv::Mat &rectified_R)
{
    if (img_L.empty() || img_R.empty())
    {
        throw std::runtime_error("rectifyImages: input images must be non-empty");
    }
    if (img_L.size() != imageSize_ || img_R.size() != imageSize_)
    {
        throw std::runtime_error("rectifyImages: input image size does not match calibration image size");
    }
    if (img_L.type() != img_R.type())
    {
        throw std::runtime_error("rectifyImages: left and right image types must match");
    }

    rectified_L = cv::Mat::zeros(imageSize_, img_L.type());
    rectified_R = cv::Mat::zeros(imageSize_, img_R.type());

    const int channels = img_L.channels();
    if (channels != 1 && channels != 3)
    {
        throw std::runtime_error("rectifyImages: only CV_8UC1 and CV_8UC3 are supported");
    }

    for (int pass = 0; pass < 2; ++pass)
    {
        const cv::Mat &src = (pass == 0) ? img_L : img_R;
        const cv::Mat &map_x = (pass == 0) ? map_L1_ : map_R1_;
        const cv::Mat &map_y = (pass == 0) ? map_L2_ : map_R2_;
        cv::Mat &dst = (pass == 0) ? rectified_L : rectified_R;

        for (int v = 0; v < imageSize_.height; ++v)
        {
            for (int u = 0; u < imageSize_.width; ++u)
            {
                const float x = map_x.at<float>(v, u);
                const float y = map_y.at<float>(v, u);

                // Border pixels that sample outside the source image stay black.
                if (x < 0.0f || y < 0.0f || x >= static_cast<float>(src.cols - 1) || y >= static_cast<float>(src.rows - 1))
                {
                    continue;
                }
                //get pixel floor values
                const int x0 = cvFloor(x);
                const int y0 = cvFloor(y);
                const int x1 = x0 + 1;
                const int y1 = y0 + 1;
                //get pixel fractional values
                const float dx = x - static_cast<float>(x0);
                const float dy = y - static_cast<float>(y0);
                //compute weights for each pixel
                const float w00 = (1.0f - dx) * (1.0f - dy);
                const float w10 = dx * (1.0f - dy);
                const float w01 = (1.0f - dx) * dy;
                const float w11 = dx * dy;
                //if greyscale, perform bilinear interpolation on the intensity value
                if (channels == 1)
                {
                    const float i00 = static_cast<float>(src.at<uchar>(y0, x0));
                    const float i10 = static_cast<float>(src.at<uchar>(y0, x1));
                    const float i01 = static_cast<float>(src.at<uchar>(y1, x0));
                    const float i11 = static_cast<float>(src.at<uchar>(y1, x1));

                    const float value = w00 * i00 + w10 * i10 + w01 * i01 + w11 * i11;
                    dst.at<uchar>(v, u) = cv::saturate_cast<uchar>(value);
                }
                //if 8UC3, perform bilinear interpolation for each color
                else
                {
                    const cv::Vec3b p00 = src.at<cv::Vec3b>(y0, x0);
                    const cv::Vec3b p10 = src.at<cv::Vec3b>(y0, x1);
                    const cv::Vec3b p01 = src.at<cv::Vec3b>(y1, x0);
                    const cv::Vec3b p11 = src.at<cv::Vec3b>(y1, x1);

                    cv::Vec3b out;
                    for (int c = 0; c < 3; ++c)
                    {
                        const float value =
                            w00 * static_cast<float>(p00[c]) +
                            w10 * static_cast<float>(p10[c]) +
                            w01 * static_cast<float>(p01[c]) +
                            w11 * static_cast<float>(p11[c]);
                        out[c] = cv::saturate_cast<uchar>(value);
                    }
                    dst.at<cv::Vec3b>(v, u) = out;
                }
            }
        }
    }
}
// PRE:  rectified_L and rectified_R are non-empty and the same size
// POST: horizontal lines are drawn across both images at regular intervals
//       the combined image is displayed in a window for visual verification
void StereoRectifier::drawEpipolarLines(cv::Mat &rectified_L, cv::Mat &rectified_R)
{
    cv::Mat combined;
    cv::hconcat(rectified_L, rectified_R, combined);

    const int step = 30;                   // pixels between lines
    const cv::Scalar lineColor(0, 255, 0); // green
    const int thickness = 1;

    for (int y = 0; y < combined.rows; y += step)
    {
        cv::line(
            combined,
            cv::Point(0, y),
            cv::Point(combined.cols - 1, y),
            lineColor,
            thickness,
            cv::LINE_AA);
    }

    cv::imshow("Rectified Pair With Epipolar Lines", combined);
    cv::waitKey(0);
}
