#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define PI 3.1415926


void canny(cv::Mat &img) {

	cv::Mat sobelx, sobely, intensity;

	// Gaussian Blur
	cv::GaussianBlur(img, img, cv::Size(5,5),0);

	// Sobel Operation (Getting sobel intensity and angle)
 	cv::Sobel(img, sobelx, -1, 1, 0, 3);
	cv::Sobel(img, sobely, -1, 0, 1, 3);
	cv::convertScaleAbs(sobelx, sobelx);
	cv::convertScaleAbs(sobely, sobely);
	cv::addWeighted(sobelx, 0.5, sobely, 0.5, 0, intensity);

	cv::Mat orientation(sobelx.size(), CV_32F);
	for(int i=0; i<sobelx.rows; i++){
		for(int j=0; j<sobelx.cols; j++){
			// std::cout << std::atan(sobely.at<float>(i,j) / sobelx.at<float>(i,j)  * 180/ PI) << std::endl;
			orientation.at<float>(i,j) = std::atan(sobely.at<float>(i,j) / sobelx.at<float>(i,j)) * 180 / PI;
			std::cout << orientation.at<float>(i,j) << " ";
		}
	}
	std::cout << std::endl;
}

int main(){
	cv::Mat img, grayImage;
	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);
	grayImage.convertTo(grayImage, CV_32F);

	canny(grayImage);

	// cv::imwrite("sobel_wo_cv.jpg", sobel_wo_cv);

	return 0;
}