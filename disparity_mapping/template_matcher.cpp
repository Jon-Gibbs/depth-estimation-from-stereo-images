#include "template_matcher.h"

// if x_L and y_L are on the edge, return
void findMatchingWindow(const cv::Mat &img_L, const cv::Mat &img_R, const int x_L, const int y_L, int &x_R, int &y_R) {
    // with rectified images, the matching window must lie in the scanline of the right image
    y_R = y_L;

    int minDiff = INT_MAX;

    int half = _windowSize / 2;

    if (x_L - half < 0 || x_L + half >= img_L.cols ||
        y_L - half < 0 || y_L + half >= img_L.rows)
        return;

    // sliding x value in the right image
    for (int sliding_X_R = half; sliding_X_R < img_L.cols - half; sliding_X_R++) {
        int ssd = 0;
        for (int i = -half; i < half; i++) {
            for (int j = -half; j < half; j++) {
                Vec3b pixel_L = img_L.at<Vec3b>(Point(x_L + i, y_L + j));
                Vec3b pixel_R = img_R.at<Vec3b>(Point(sliding_X_R + i, y_R + j));

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

// should be odd number
TemplateMatcher(int windowSize) {
    _windowSize = windowSize;
}