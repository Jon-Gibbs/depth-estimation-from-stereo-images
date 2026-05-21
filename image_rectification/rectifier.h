#include <opencv2/opencv.hpp>
#include <vector>
#include <utility> // For std::pair
class rectifier
{
public:
    rectifier();
    // precondition:
    std::vector<std::pair<int, int>> find_matching_points(sl::Mat img1, sl::Mat img2);
};