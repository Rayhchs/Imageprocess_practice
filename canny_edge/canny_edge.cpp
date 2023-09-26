#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define PI 3.1415926

enum dir{up, right, down, left, zero};

void canny(cv::Mat &img) {

	cv::Mat sobelx, sobely, sobelx_abs, sobely_abs, intensity;

	// Gaussian Blur
	cv::GaussianBlur(img, img, cv::Size(5,5),0);

	// Sobel Operation (Getting sobel intensity and angle)
 	cv::Sobel(img, sobelx, -1, 1, 0, 3);
	cv::Sobel(img, sobely, -1, 0, 1, 3);
	cv::convertScaleAbs(sobelx, sobelx_abs);
	cv::convertScaleAbs(sobely, sobely_abs);
	cv::addWeighted(sobelx_abs, 0.5, sobely_abs, 0.5, 0, intensity);

	cv::Mat orientation(sobelx.size(), CV_32F);
	for(int i=0; i<sobelx.rows; i++){
		for(int j=0; j<sobelx.cols; j++){
			float x = (sobelx.at<float>(i,j) != 0) ? sobelx.at<float>(i,j) : 10e-5;
			float angle = std::atan(sobely.at<float>(i,j) / x) * (180 / PI);
			int a = angle;
			if ((angle <=22.5 && angle >=0) || (angle >=157.5 && angle <=180)){
				orientation.at<int>(i,j) = up;
			}
			else if ((angle <22.5 && angle >=67.5)){
				orientation.at<int>(i,j) = right;
			}
			else if ((angle <67.5 && angle >=112.5)){
				orientation.at<int>(i,j) = down;
			}
			else if ((angle <112.5 && angle >=157.5)){
				orientation.at<int>(i,j) = down;
			}
			else{
				orientation.at<int>(i,j) = zero;
			}
			// orientation.at<float>(i,j) = std::atan(sobely.at<float>(i,j) / x) * (180 / PI);
			std::cout << orientation.at<int>(i,j) << " " << a << ";";
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