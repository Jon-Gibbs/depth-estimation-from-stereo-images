#include "template_matcher.h"
#include <opencv2/opencv.hpp>


// if x_L and y_L are on the edge, return
void TemplateMatcher::findMatchingWindow(const cv::Mat &img_L, const cv::Mat &img_R, 
                        const int x_L, const int y_L, int &x_R, int &y_R) {
    
    // with rectified images, the matching window must lie in the scanline of the right image
    y_R = y_L;

    int minDiff = INT_MAX;

    int half = _windowSize / 2;

    if (x_L - half < 0 || x_L + half >= img_L.cols ||
        y_L - half < 0 || y_L + half >= img_L.rows)
        return;

    // sliding x value in the right image
    for (int sliding_X_R = std::max(half, x_L - _maxDisparity);
         sliding_X_R < std::min(img_R.cols - half, x_L + _maxDisparity);
         sliding_X_R++) {
        int ssd = 0;
        for (int i = -half; i < half; i++) {
            for (int j = -half; j < half; j++) {
                cv::Vec3b pixel_L = img_L.at<cv::Vec3b>(cv::Point(x_L + i, y_L + j));
                cv::Vec3b pixel_R = img_R.at<cv::Vec3b>(cv::Point(sliding_X_R + i, y_R + j));

                // find the difference between the two pixels in the left and right windows
                int dR = (int)pixel_L[0] - (int)pixel_R[0];
                int dG = (int)pixel_L[1] - (int)pixel_R[1];
                int dB = (int)pixel_L[2] - (int)pixel_R[2];

                ssd += dR*dR + dG*dG + dB*dB;
            }
        }
        if (ssd < minDiff) {
            minDiff = ssd;
            x_R = sliding_X_R;
        }
    }
}

void TemplateMatcher::drawMatchingPoints(const cv::Mat &img_L, const cv::Mat &img_R, 
                        const int x_L, const int y_L, 
                        const int x_R, const int y_R,
                        cv::Mat &output_L, cv::Mat &output_R) {
    output_L = img_L.clone();
    output_R = img_R.clone();
    cv::circle(
        output_L,                // Image to draw on
        cv::Point(x_L, y_L),              // Center point
        3,                  // Radius in pixels
        cv::Scalar(0, 0, 255), // Color (B, G, R) → Red
        cv::FILLED,         // Fill the circle
        cv::LINE_AA         // Anti-aliased line
    );

    cv::circle(
        output_R,                // Image to draw on
        cv::Point(x_R, y_R),              // Center point
        3,                  // Radius in pixels
        cv::Scalar(0, 0, 255), // Color (B, G, R) → Red
        cv::FILLED,         // Fill the circle
        cv::LINE_AA         // Anti-aliased line
    );

}

// should be odd number
TemplateMatcher::TemplateMatcher(int windowSize, int maxDisparity) {
    _windowSize = windowSize;
    _maxDisparity = maxDisparity;
}