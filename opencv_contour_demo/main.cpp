#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

int main()
{
    Mat src = imread("/home/zhengyuyuan/opencv_contour_demo/test.jpg.jpg");
    if (src.empty())
    {
        cout << "图片读取失败，请检查图片名称和路径！" << endl;
        return -1;
    }

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    Mat blur;
    GaussianBlur(gray, blur, Size(3,3), 0);

    Mat edge;
    Canny(blur, edge, 50, 150);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edge, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    Mat lineCanvas = Mat::zeros(src.size(), CV_8UC3);
    lineCanvas.setTo(Scalar(255,255,255));
    drawContours(lineCanvas, contours, -1, Scalar(0,0,0), 1);

    imwrite("/home/zhengyuyuan/opencv_contour_demo/line_result.png", lineCanvas);
    cout << "成功！线稿已保存 line_result.png" << endl;

    return 0;
}
