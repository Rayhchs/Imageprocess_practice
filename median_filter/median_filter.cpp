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
};

void median_filter(func_in in) {

	// Median filter without OpenCV
	std::vector<std::vector<int>> vecimg;
	for (int i=0; i<in.img.rows; i++){
		std::vector<int> rowValues;
		for (int j=0; j<in.img.cols; j++){
			std::vector<int> kernel;

			// Set boundary to prevent out of bound error
			int b1, b2, b3, b4;
			b1 = (i-(in.k_size/2) >= 0) ? -(in.k_size/2) : 0;
			b2 = (j-(in.k_size/2) >= 0) ? -(in.k_size/2) : 0;
			b3 = (i+(in.k_size/2) < in.img.rows) ? (in.k_size/2)+1 : 0;
			b4 = (j+(in.k_size/2) < in.img.cols) ? (in.k_size/2)+1 : 0;
			for (int k1=b1; k1<b3; k1++){
				for (int k2=b2; k2<b4; k2++){
					kernel.push_back(in.img.at<uchar>(i+k1, j+k2));
				}
			}
			sort(kernel.begin(), kernel.end());
			rowValues.push_back(kernel[(in.k_size*in.k_size)/2]);
		}
		vecimg.push_back(rowValues);
	}

	// Put vector value back to cv::Mat
	for (int i = 0; i < in.img.rows; i++) {
	    for (int j = 0; j < in.img.cols; j++) {
	        in.img.at<int>(i, j, 0) = vecimg[i][j];
	    }
	}
}

int main(){
	cv::Mat img, filtered_img, rChannel, gChannel, bChannel;
	img = cv::imread("../Lenna.jpg", cv::IMREAD_COLOR);

	// W OpenCV
	cv::medianBlur(img, filtered_img, 3);
	cv::imwrite("filtered_w_cv.jpg", filtered_img);

	// WO OpenCV
	cv::Mat filtered_img_2;
	std::vector<cv::Mat> channels;
	cv::split(img, channels);

	// A thread for a channel
	std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
    	func_in in;
    	in.k_size = 3;
    	in.img = channels[i];
        threads.emplace_back(median_filter, in);
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // Merge RGB channels
	cv::merge(channels, filtered_img_2);
	cv::imwrite("filtered_wo_cv.jpg", filtered_img_2);

	return 0;
}