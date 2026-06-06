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

The project is configured for the **MSYS2 UCRT64** toolchain on Windows. The steps below use the MSYS2 UCRT64 shell throughout.

### Prerequisites

1. **MSYS2** — download and install from [https://www.msys2.org](https://www.msys2.org)
2. Open the **UCRT64** shell (`C:\msys64\ucrt64.exe`) and install the required packages:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-opencv \
          mingw-w64-ucrt-x86_64-yaml-cpp
```

> **Camera module only:** also requires the [ZED SDK](https://www.stereolabs.com/developers/) and a compatible CUDA installation. The `camera/` module will not build without them.

---

### Building the full stereo pipeline (`launch/`)

This is the recommended build target — it compiles `image_rectification` and `disparity_mapping` together into a single `stereo_pipeline` executable.

```bash
cd /c/Users/<your-username>/Coding_Projects/Stero_Image_Depth_Map/depth-estimation-from-stereo-images/launch
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
```

Run the pipeline:

```bash
./stereo_pipeline
```

---

### Building individual modules

#### Image Rectification

```bash
cd image_rectification
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
./stereo_rectifier
```

#### Disparity Mapping

```bash
cd disparity_mapping
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
./disparity_mapper
```

#### Camera (requires ZED SDK + CUDA)

```bash
cd camera
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
```

---

### Notes

- All commands must be run inside the **MSYS2 UCRT64** shell, not PowerShell or CMD.
- If OpenCV is installed to a non-default location, pass `-DOpenCV_DIR=<path/to/opencv/cmake>` to the `cmake` command.
- Output depth maps are written to `depth_maps/` and disparity maps to `disparity_maps/`.

## Build for Linux and Mac

Open a terminal in the /launch directory and execute the following commands:

    cmake .
    make
    ./stereo_pipeline
