# Depth estimation using stereo images

This program takes a pair of images from the Sterolabs Zed2i camera and turns them into a depth map.

## Camera

- The Camera module collects images from the camera as well as intinsics/extrinsics from the Zed SDK and stores them in intrinsics.yaml
  -The camera info is stored in camera/camera_info.yaml and the images are stored in unrectified_images/
- For more information on the Zed SDK, read the ZED [API documentation](https://www.stereolabs.com/developers/documentation/API/).

## Rectification

- The rectification module (/image_rectifiction) reads the camera info from camera/camera_info.yaml as well as images from /unrectified_images and produces pairs of rectified images that are stored in /rectified_images

## Disparity Mapping

- The Disparity Mapping module takes a rectified pair of images and uses the SGBM algorithm to produce a disparity map

## Build for Windows

- Create a "build" folder in the source folder of the module you want to build in
- Open cmake-gui and select the source and build folders
- Generate the Visual Studio `Win64` solution
- Open the resulting solution and change configuration to `Release`
- Build solution
- to build, cd into the /build directory, run cmake . then make and execute ./depth mapper
- upon output, you will see 10 depth maps of 10 different images including their original image pairs

## Build for Linux

Open a terminal in the sample directory and execute the following command:

    mkdir build
    cd build
    cmake ..
    make
