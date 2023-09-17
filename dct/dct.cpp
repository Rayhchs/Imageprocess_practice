#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


void LPF_process(cv::Mat &dct_matrix, float cutoffFreq) {
    // Get h, w of dct matrix
    int h = dct_matrix.rows;
    int w = dct_matrix.cols;

    // Create a low pass filter
    cv::Mat lowPassFilter = cv::Mat::zeros(h, w, CV_32F);
    int cutoffX = cutoffFreq * w;
    int cutoffY = cutoffFreq * h;
    lowPassFilter(cv::Rect(0, 0, cutoffX, cutoffY)) = 1.0;

    // Apply the filter
    cv::multiply(dct_matrix, lowPassFilter, dct_matrix);
}

int main(){
	cv::Mat img, grayImage, dct_matrix, idct_img;
	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);
	img.convertTo(img, CV_32F, 1.0/255.0);
	cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);

	// Discrete Cosine Transform
	cv::dct(grayImage, dct_matrix);
	cv::Mat dct_matrix_copy = dct_matrix.clone();

	// Apply low pass filter with cutoff frequency = 0.25
	LPF_process(dct_matrix, 0.4);

	// Inverse Discrete Cosine Transform
	cv::idct(dct_matrix, idct_img);

	// Result
	cv::Mat out;
	cv::hconcat(grayImage, idct_img, grayImage);
	cv::hconcat(dct_matrix_copy, dct_matrix, dct_matrix_copy);
	cv::vconcat(grayImage, dct_matrix_copy, out);
	cv::imshow("dct_lpf", out);
	cv::waitKey(0);
	cv::imwrite("dct_lpf.jpg", out*255);
	return 0;
}