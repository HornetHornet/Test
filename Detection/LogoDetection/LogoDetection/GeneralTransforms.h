#pragma once

#ifndef GENERALTRAMSFORSM_H
#define GENERALTRAMSFORSM_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace trnsf 
{
	//resize so that the max dimention be equal to "size"
	static void preciseResize(Mat &image, double size) {
		double resizeFactor = min(size / image.rows, size / image.cols);
		cv::resize(image, image, Size(), resizeFactor, resizeFactor, cv::INTER_CUBIC);
	};

	//resize down if one of image dimensions is more than "size"
	static void resizeDown(Mat &image, double size) {
		if (max(image.rows, image.cols) > size)
			preciseResize(image, size);
	};

	//resize up if image dimensions is less than "size"
	static void resizeUp(Mat &image, double size) {
		if (max(image.rows, image.cols) < size)
			preciseResize(image, size);
	};

	//replace transparent with white
	static void makeOpaque(Mat &image) {

		Mat channels[4];

		split(image, channels);
		bitwise_not(channels[3], channels[3]);
		//channels[3].convertTo(channels[3], -1, 1, -128);

		for (size_t i = 0; i < 3; i++)
			add(channels[i], channels[3], channels[i]);

		merge(channels, 3, image);
	};
}

#endif