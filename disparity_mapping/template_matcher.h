#include <opencv2/opencv.hpp>
//#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>
#include <climits>

class TemplateMatcher {
public:
    TemplateMatcher(int windowSize, int maxDisparity);
    void findMatchingWindow(const cv::Mat &img_L, const cv::Mat &img_R, 
                        const int x_L, const int y_L, int &x_R, int &y_R);

    void drawMatchingPoints(const cv::Mat &img_L, const cv::Mat &img_R, 
                        const int x_L, const int y_L, 
                        const int x_R, const int y_R,
                        cv::Mat &output_L, cv::Mat &output_R);

private:
    int _windowSize;
    int _maxDisparity;
};