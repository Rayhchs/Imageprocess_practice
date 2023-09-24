#include<iostream>
#include<vector>
#include <numeric>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

struct func_in{
	cv::Mat img;
	int k_size;
	int dir;
};

void sobel(func_in in) {

	// Create Sobel matrix
	int s = in.k_size;
	int kernelx[s][s];
	int kernely[s][s];
    for (int i = 0; i < s; i++) {
        for (int j = 0; j < s; j++) {
    		kernelx[i][j] = (s/2 != j) ? (s/2-i) : (s/2-i)*2;
    		kernely[i][j] = (s/2 != i) ? (s/2-j) : (s/2-j)*2;
        }
    }

	// Sobel without OpenCV
 	std::vector<std::vector<uchar>> vecimg;
	for (int i=0; i<in.img.rows; i++){
		std::vector<uchar> rowValues;
		for (int j=0; j<in.img.cols; j++){

			// Set boundary to prevent out of bound error
			int b1, b2, b3, b4, valx=0, valy=0;
			b1 = -(s/2);
			b2 = -(s/2);
			b3 = (s/2)+1;
			b4 = (s/2)+1;
			if (i+b1 >= 0 && i+b3 < in.img.rows && j+b2 >= 0 && j+b4 < in.img.cols) {
				for (int k1=b1; k1<b3; k1++){
					for (int k2=b2; k2<b4; k2++){
						valx += (in.img.at<uchar>(i+k1, j+k2) * kernelx[s/2+k1][s/2+k2]);
						valy += (in.img.at<uchar>(i+k1, j+k2) * kernely[s/2+k1][s/2+k2]);
					}
				}
			}
			else{
				valx = 0;
				valy = 0;
			}
			valx = std::sqrt(std::pow(valx, 2));
			valy = std::sqrt(std::pow(valy, 2));
			if (valx > 255) valx=255;
			else if (valx < 0) valx=0;
			if (valy > 255) valy=255;
			else if (valy < 0) valy=0;
			int val = (valx + valy) / 2;
			rowValues.push_back(val);
		}
		vecimg.push_back(rowValues);
	}

	// Put vector value back to cv::Mat
	for (int i = 0; i < in.img.rows; i++) {
	    for (int j = 0; j < in.img.cols; j++) {
	    	in.img.at<uchar>(i, j) = static_cast<uchar>(vecimg[i][j]);
	    }
	}
}

int main(){
	cv::Mat img, grayImage;
	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);

	// W OpenCV
	cv::Mat x_sobel_cv, y_sobel_cv, sobel_cv;
	cv::Sobel(grayImage, x_sobel_cv, CV_16S, 1, 0, 3);
	cv::Sobel(grayImage, y_sobel_cv, CV_16S, 0, 1, 3);
	cv::convertScaleAbs(x_sobel_cv, x_sobel_cv);
	cv::convertScaleAbs(y_sobel_cv, y_sobel_cv);
	cv::imwrite("t.jpg", y_sobel_cv);
	cv::addWeighted(x_sobel_cv, 0.5, y_sobel_cv, 0.5, 0, sobel_cv);
	cv::imwrite("sobel_w_cv.jpg", sobel_cv);

	// WO OpenCV
	cv::Mat sobel_wo_cv=grayImage;

	func_in in;
	in.k_size = 3;
	in.img = sobel_wo_cv;
	sobel(in);
	cv::imwrite("sobel_wo_cv.jpg", sobel_wo_cv);

	return 0;
}