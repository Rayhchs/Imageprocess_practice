#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Mat circle_detect(cv::Mat &img, cv::Mat &out) {



}

int main(){
	cv::Mat img, grayImage, canny_cv;

	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(grayImage, grayImage, cv::Size(5,5),0);
	cv::Mat canny_cv;
	cv::Canny(grayImage, canny_cv, 30, 60);
	cv::imwrite("canny_edge_cv.jpg", canny_cv);

	return 0;
}