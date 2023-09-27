#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define PI 3.1415926

enum dir{vertical=0, right=1, horizontal=2, left=3, zero=4};

void scaleToUChar(const cv::Mat& src, cv::Mat& dst, float max_rato) {
    src.convertTo(dst, CV_32F);
    double maxValue;
    minMaxLoc(dst, 0, &maxValue);
    
    int bins = 100;
    int histSize = bins;
    float range[] = { 0, float(maxValue) };
    const float* ranges[] = { range };
    cv::Mat hist;
    calcHist(&dst, 1, 0, cv::Mat(), hist, 1, &histSize, ranges, true, false);

    int threshold = int(sum(hist.rowRange(5, bins-1))[0] * max_rato);
    int sum = 0;
    int i = bins-1;
    while (i >= 0) {
        sum += hist.at<float>(i);
        if (sum > threshold) break;
        i--;
    }
    float scaleRatio = (255 * bins) / ( i * maxValue);
    convertScaleAbs(dst, dst, scaleRatio);
}

cv::Mat canny(cv::Mat &img, int thres_l, int thres_h) {

	cv::Mat sobelx, sobely, sobelx_abs, sobely_abs, intensity;

	// Gaussian Blur
	cv::GaussianBlur(img, img, cv::Size(5,5),0);

	// Sobel Operation (Getting sobel intensity and angle)
 	cv::Sobel(img, sobelx, -1, 1, 0, 3);
	cv::Sobel(img, sobely, -1, 0, 1, 3);
	cv::pow(sobelx, 2, sobelx);
	cv::pow(sobely, 2, sobely);
	cv::add(sobelx, sobely, intensity);
	cv::pow(intensity, 0.5, intensity);
	// scaleToUChar(intensity, intensity, 0.2);
	intensity.convertTo(intensity, CV_8UC1);
	// cv::convertScaleAbs(sobelx, sobelx_abs);
	// cv::convertScaleAbs(sobely, sobely_abs);
	// cv::addWeighted(sobelx_abs, 0.5, sobely_abs, 0.5, 0, intensity);

	// Sobel Orientation Gy/Gx
	double tan225 = 0.414, tan675 = 2.414;
	cv::Mat orientation(img.size(), CV_32F);
	for(int j=0; j<sobelx.rows; j++){
		for(int i=0; i<sobelx.cols; i++){
			double x = sobelx.at<double>(i,j) + 1e-5;
			double angle = sobely.at<double>(i,j) / x;
			if ((angle < tan675 && angle >= tan225)){
				orientation.at<int>(i,j) = dir::right;
			}
			else if ((angle < tan225 && angle >= -tan225)){
				orientation.at<int>(i,j) = dir::horizontal;
			}
			else if ((angle < -tan225 && angle >= -tan675)){
				orientation.at<int>(i,j) = dir::left;
			}
			else {
				orientation.at<int>(i,j) = dir::vertical;
			}
		}
	}

	// NMS
	for (int y=1; y<orientation.rows-1; y++){
		for(int x=1; x<orientation.cols-1; x++){
			uchar v1, v2;
			int dir = orientation.at<int>(x,y);
			uchar pixel = intensity.at<uchar>(x,y);
			switch (dir) {
				case dir::vertical:
					v1 = intensity.at<uchar>(x+1,y);
					v2 = intensity.at<uchar>(x-1,y);
					break;
				case dir::horizontal:
					v1 = intensity.at<uchar>(x,y+1);
					v2 = intensity.at<uchar>(x,y-1);
					break;
				case dir::right:
					v1 = intensity.at<uchar>(x+1,y-1);
					v2 = intensity.at<uchar>(x-1,y+1);
					break;
				case dir::left:
					v1 = intensity.at<uchar>(x+1,y+1);
					v2 = intensity.at<uchar>(x-1,y-1);
					break;
				default:
					v1 = 255;
					v2 = 255;
					break;
			}
			if (pixel <= v1 || pixel <= v2) {
				intensity.at<uchar>(x,y) = 0;
			}
		}
	}

	// Thresholding
	cv::Mat high, low;
	cv::threshold(intensity, high, thres_h, 255, cv::THRESH_BINARY);
    cv::threshold(intensity, low, thres_l, 255, cv::THRESH_BINARY);
	for (int y=1; y<low.rows-1; y++){
		for(int x=1; x<low.cols-1; x++){
			uchar pixel = low.at<uchar>(x,y);
			uchar pixel_h = high.at<uchar>(x,y);
			if (pixel > 0 && pixel_h == 0){
				std::vector<uchar> h = {high.at<uchar>(x-1,y-1), high.at<uchar>(x,y-1), high.at<uchar>(x+1,y-1),
										high.at<uchar>(x-1,y), high.at<uchar>(x,y), high.at<uchar>(x+1,y),
										high.at<uchar>(x-1,y+1), high.at<uchar>(x,y+1), high.at<uchar>(x+1,y+1)};
				// for (int k=0; k<h.size(); k++){
				// 	if (h[k] > 0){
				// 		high.at<uchar>(x,y) = 255;
				// 		break;
				// 	}
				// }
				uchar v1, v2;
				int dir = orientation.at<int>(x,y);
				switch (dir) {
					case dir::vertical:
						v1 = high.at<uchar>(x+1,y);
						v2 = high.at<uchar>(x-1,y);
						break;
					case dir::horizontal:
						v1 = high.at<uchar>(x,y+1);
						v2 = high.at<uchar>(x,y-1);
						break;
					case dir::right:
						v1 = high.at<uchar>(x+1,y-1);
						v2 = high.at<uchar>(x-1,y+1);
						break;
					case dir::left:
						v1 = high.at<uchar>(x+1,y+1);
						v2 = high.at<uchar>(x-1,y-1);
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
	grayImage.convertTo(grayImage, CV_64F);

	canny_edge = canny(grayImage, 30, 60);
	cv::imwrite("canny_edge.jpg", canny_edge);

	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(grayImage, grayImage, cv::Size(5,5),0);
	cv::Mat canny_cv;
	cv::Canny(grayImage, canny_cv, 30, 60);
	cv::imwrite("canny_edge_cv.jpg", canny_cv);

	return 0;
}