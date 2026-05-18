///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include <sl/Camera.hpp>
#include <string>

using namespace sl;

int main(int argc, char** argv) {

    // Create a ZED camera object
    Camera zed;

    // Open the camera
    ERROR_CODE returned_state = zed.open();
    if (returned_state > ERROR_CODE::SUCCESS) {
        std::cout << "Error " << returned_state << ", exit program.\n";
        return EXIT_FAILURE;
    }

    // Get camera information (ZED serial number)
    auto camera_infos = zed.getCameraInformation();
    printf("Hello! This is my serial number: %d\n", camera_infos.serial_number);
    // Capture 50 frames and stop
    int i = 0;
    sl::Mat left_image;
    sl::Mat right_image;
    while (i < 10) {
        // Grab an image
        if (zed.grab() == ERROR_CODE::SUCCESS) {
            // A new image is available if grab() returns ERROR_CODE::SUCCESS
            zed.retrieveImage(left_image, VIEW::LEFT); // Get the left image
            zed.retrieveImage(right_image, VIEW::RIGHT); // Get the right image
            auto timestamp = zed.getTimestamp(sl::TIME_REFERENCE::IMAGE); // Get image timestamp
            printf("Image resolution: %zu x %zu  || Image timestamp: %llu\n",
                   left_image.getWidth(),
                   left_image.getHeight(),
                   static_cast<unsigned long long>(timestamp));
            i++;

            const sl::String& left_img_path = "images/left_frame_" + std::to_string(i) + ".jpg";
            const sl::String& right_img_path = "images/right_frame_" + std::to_string(i) + ".jpg";
            left_image.write(left_img_path);
            right_image.write(right_img_path);
        }
    }
    // Close the camera
    zed.close();
    return EXIT_SUCCESS;
}
