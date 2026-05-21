#include <sl/Camera.hpp>
#include <iostream>
using namespace sl;
int main()
{
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

    // Right camera intrinsics
    std::cout << "Right fx: " << calib.right_cam.fx << "\n";
    std::cout << "Right fy: " << calib.right_cam.fy << "\n";
    std::cout << "Right cx: " << calib.right_cam.cx << "\n";
    std::cout << "Right cy: " << calib.right_cam.cy << "\n";

    // Distortion coefficients (vector)
    std::cout << "Left disto: ";
    for (const auto &d : calib.left_cam.disto)
        std::cout << d << " ";
    std::cout << "\nRight disto: ";
    for (const auto &d : calib.right_cam.disto)
        std::cout << d << " ";
    std::cout << "\n";

    // Stereo baseline (translation between cameras)
    std::cout << "Baseline T.x: " << calib.T.x << " (units depend on SDK struct)\n";

    zed.close();
    return 0;
}