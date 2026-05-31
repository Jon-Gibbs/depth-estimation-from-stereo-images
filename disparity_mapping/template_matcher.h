#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>
#include <climits>

class TemplateMatcher {
public:
    TemplateMatcher(int windowSize);
    void findMatchingWindow(const cv::Mat &img_L, const cv::Mat &img_R, const int x_L, const int y_L, int &x_R, int &y_R);

private:
    int _windowSize;
}