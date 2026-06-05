#include <sl/Camera.hpp>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace sl;
int main()
{
    YAML::Node config = YAML::LoadFile("intrinsics.yaml");

    sl::Camera zed;
    sl::InitParameters init_params;
    init_params.camera_resolution = sl::RESOLUTION::HD720;
    init_params.camera_fps = 30;

    auto err = zed.open(init_params);
    if (err != sl::ERROR_CODE::SUCCESS)
    {
        std::cerr << "ZED open failed: " << sl::toString(err) << "\n";
        return 1;
    }

    auto info = zed.getCameraInformation();

    // Calibration matched to the currently opened mode/resolution
    const auto &calib = info.camera_configuration.calibration_parameters;

    // Left camera intrinsics
    std::cout << "Left fx: " << calib.left_cam.fx << "\n";
    std::cout << "Left fy: " << calib.left_cam.fy << "\n";
    std::cout << "Left cx: " << calib.left_cam.cx << "\n";
    std::cout << "Left cy: " << calib.left_cam.cy << "\n";
    config["left_camera"]["intrinsics"]["fx"] = calib.left_cam.fx;
    config["left_camera"]["intrinsics"]["fy"] = calib.left_cam.fy;
    config["left_camera"]["intrinsics"]["cx"] = calib.left_cam.cx;
    config["left_camera"]["intrinsics"]["cy"] = calib.left_cam.cy;
    // Distortion: [k1, k2, p1, p2, k3]
    std::cout << "Distortion coefficients:" << std::endl;
    std::cout << "  k1: " << calib.left_cam.disto[0] << std::endl;
    std::cout << "  k2: " << calib.left_cam.disto[1] << std::endl;
    std::cout << "  p1: " << calib.left_cam.disto[2] << std::endl;
    std::cout << "  p2: " << calib.left_cam.disto[3] << std::endl;
    std::cout << "  k3: " << calib.left_cam.disto[4] << std::endl;
    config["left_camera"]["disto"]["k1"] = calib.left_cam.disto[0];
    config["left_camera"]["disto"]["k2"] = calib.left_cam.disto[1];
    config["left_camera"]["disto"]["p1"] = calib.left_cam.disto[2];
    config["left_camera"]["disto"]["p2"] = calib.left_cam.disto[3];
    config["left_camera"]["disto"]["k3"] = calib.left_cam.disto[4];

    // Right camera intrinsics
    std::cout << "Right fx: " << calib.right_cam.fx << "\n";
    std::cout << "Right fy: " << calib.right_cam.fy << "\n";
    std::cout << "Right cx: " << calib.right_cam.cx << "\n";
    std::cout << "Right cy: " << calib.right_cam.cy << "\n";
    config["right_camera"]["intrinsics"]["fx"] = calib.right_cam.fx;
    config["right_camera"]["intrinsics"]["fy"] = calib.right_cam.fy;
    config["right_camera"]["intrinsics"]["cx"] = calib.right_cam.cx;
    config["right_camera"]["intrinsics"]["cy"] = calib.right_cam.cy;
    // Distortion: [k1, k2, p1, p2, k3]
    std::cout << "Distortion coefficients:" << std::endl;
    std::cout << "  k1: " << calib.right_cam.disto[0] << std::endl;
    std::cout << "  k2: " << calib.right_cam.disto[1] << std::endl;
    std::cout << "  p1: " << calib.right_cam.disto[2] << std::endl;
    std::cout << "  p2: " << calib.right_cam.disto[3] << std::endl;
    std::cout << "  k3: " << calib.right_cam.disto[4] << std::endl;
    config["right_camera"]["disto"]["k1"] = calib.right_cam.disto[0];
    config["right_camera"]["disto"]["k2"] = calib.right_cam.disto[1];
    config["right_camera"]["disto"]["p1"] = calib.right_cam.disto[2];
    config["right_camera"]["disto"]["p2"] = calib.right_cam.disto[3];
    config["right_camera"]["disto"]["k3"] = calib.right_cam.disto[4];

    '''
    CalibrationParameters calibration_params = zed.getCameraInformation().camera_configuration.calibration_parameters;
// Focal length of the left eye in pixels
float focal_left_x = calibration_params.left_cam.fx;
// First radial distortion coefficient
float k1 = calibration_params.left_cam.disto[0];
// Translation between left and right eye on x-axis
float tx = calibration_params.stereo_transform.getTranslation()[0];
// Horizontal field of view of the left eye in degrees
float h_fov = calibration_params.left_cam.h_fov;
    '''
    // Stereo baseline (translation between cameras)
    std::cout << "Baseline T.x: " << calib.T.x << std::endl;
    config["translation"]["x_pos"] = calib.stereo_transform.getTranslation()[0];
    config["translation"]["y_pos"] = calib.stereo_transform.getTranslation()[1];
    config["translation"]["z_pos"] = calib.stereo_transform.getTranslation()[2];

    // Stero rotation (rotation between cameras)
    std::cout << "Rotation x: " << calib.stereo_transform.getRotationMatrix()[0][0]; << std::endl;
    std::cout << "Rotation y: " << calib.stereo_transform.getRotationMatrix()[1][1]; << std::endl;
    std::cout << "Rotation z: " << calib.stereo_transform.getRotationMatrix(); << std::endl;
    sl::Rotation R = calib.stereo_transform.getRotationMatrix();
    for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        std::cout << R(i, j) << " ";
    }
    std::cout << "\n";
}
    config["rotation"]["x"] = calib.R.x;
    config["rotation"]["y"] = calib.R.y;
    config["rotation"]["z"] = calib.R.z;

    std::ofstream fout("instrinsics.yaml");

    fout << config;
    zed.close();
    return 0;
}