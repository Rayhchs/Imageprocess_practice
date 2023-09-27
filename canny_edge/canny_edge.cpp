#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

enum dir{vertical=0, right=1, horizontal=2, left=3, zero=4};

cv::Mat canny(cv::Mat &img, int thres_l, int thres_h) {

	cv::Mat sobelx, sobely, intensity;

	// Gaussian Blur
	cv::GaussianBlur(img, img, cv::Size(5,5),0);
	img.convertTo(img, CV_32F);

	// Sobel Operation (Getting sobel intensity and angle)
 	cv::Sobel(img, sobelx, -1, 1, 0, 3);
	cv::Sobel(img, sobely, -1, 0, 1, 3);
	cv::pow(sobelx, 2, sobelx);
	cv::pow(sobely, 2, sobely);
	cv::add(sobelx, sobely, intensity);
	cv::pow(intensity, 0.5, intensity);
	intensity.convertTo(intensity, CV_8U);

	// Sobel Orientation Gy/Gx
	double tan225 = 0.4142135, tan675 = 2.4142135;
	cv::Mat orientation(img.size(), CV_32F);
	for(int i=0; i<sobelx.rows; i++){
		for(int j=0; j<sobelx.cols; j++){
			double x = sobelx.at<double>(i,j) + 1e-5;
			double angle = sobely.at<double>(i,j) / x;
			if ((angle < tan675 && angle >= tan225)){
				orientation.at<int>(i,j) = (int) dir::right;
			}
			else if ((angle < tan225 && angle >= -tan225)){
				orientation.at<int>(i,j) = (int) dir::horizontal;
			}
			else if ((angle < -tan225 && angle >= -tan675)){
				orientation.at<int>(i,j) = (int) dir::left;
			}
			else {
				orientation.at<int>(i,j) = (int) dir::vertical;
			}
			std::cout << orientation.at<int>(i,j) << " ";
		}
	}
	orientation.convertTo(orientation, CV_8U);

	// NMS
	for (int x=1; x<orientation.rows-1; x++){
		for(int y=1; y<orientation.cols-1; y++){
			uchar v1, v2;
			int direction = orientation.at<int>(x,y);
			uchar pixel = intensity.at<uchar>(x,y);
			switch (direction) {
				case dir::vertical:
					v1 = intensity.at<uchar>(x,y+1);
					v2 = intensity.at<uchar>(x,y-1);
					break;
				case dir::horizontal:
					std::cout << direction << " ";
					v1 = intensity.at<uchar>(x+1,y);
					v2 = intensity.at<uchar>(x-1,y);
					break;
				case dir::right:
					v1 = intensity.at<uchar>(x+1,y+1);
					v2 = intensity.at<uchar>(x-1,y-1);
					break;
				case dir::left:
					v1 = intensity.at<uchar>(x+1,y-1);
					v2 = intensity.at<uchar>(x-1,y+1);
					break;
				default:
					v1 = 255;
					v2 = 255;
					break;
			}
			if (pixel < v1 || pixel < v2) {
				intensity.at<uchar>(x,y) = 0;
			}
		}
	}

	// Thresholding
	cv::Mat high, low;
	cv::threshold(intensity, high, thres_h, 255, cv::THRESH_BINARY);
    cv::threshold(intensity, low, thres_l, 255, cv::THRESH_BINARY);

	for (int x=1; x<low.rows-1; x++){
		for(int y=1; y<low.cols-1; y++){
			uchar pixel = low.at<uchar>(x,y);
			uchar pixel_h = high.at<uchar>(x,y);
			if (pixel > 0 && pixel_h == 0){
				uchar v1, v2;
				int direction = orientation.at<int>(x,y);
				switch (direction) {
					case dir::vertical:
						v1 = high.at<uchar>(x,y+1);
						v2 = high.at<uchar>(x,y-1);
						break;
					case dir::horizontal:
						v1 = high.at<uchar>(x+1,y);
						v2 = high.at<uchar>(x-1,y);
						break;
					case dir::right:
						v1 = high.at<uchar>(x+1,y+1);
						v2 = high.at<uchar>(x-1,y-1);
						break;
					case dir::left:
						v1 = high.at<uchar>(x+1,y-1);
						v2 = high.at<uchar>(x-1,y+1);
						break;
					default:
						v1 = 0;
						v2 = 0;
						break;
				}
				if (v1 > 0 && v2 > 0) {
					high.at<uchar>(x,y) = 255;
				}
			}
		}
	}
	return high;
}

int main(){
	cv::Mat img, grayImage, canny_edge;
	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);

	canny_edge = canny(grayImage, 30, 60);
	cv::imwrite("canny_edge.jpg", canny_edge);



	return 0;
}