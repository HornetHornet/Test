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

	//apply bilateralFilter
	static void filter(Mat &image, int filteringKernel = 9) {
			Mat imCopy = image.clone();
			bilateralFilter(image, imCopy, filteringKernel, filteringKernel * 2, filteringKernel / 2);
			// 	image = imCopy.clone(); medianBlur(image, image, 3);
	}

	// reduce colors in div times
	static void reduce(Mat& image, int div) {

		static int divideWith;
		static uchar table[256];
		static Mat lookUpTable(1, 256, CV_8U);
		static uchar* p = lookUpTable.data;

		if (divideWith != div) {
			divideWith = div;
			for (int i = 0; i < 256; ++i)
				table[i] = (uchar)(divideWith * (i / divideWith));

			for (int i = 0; i < 256; ++i)
				p[i] = table[i];
		}

		LUT(image, lookUpTable, image);
	}

	//replace transparent with grey
	static void makeOpaque(Mat &image) {

		Mat channels[4];

		split(image, channels);
		bitwise_not(channels[3], channels[3]);
		channels[3].convertTo(channels[3], -1, 1, -128);

		for (size_t i = 0; i < 3; i++)
			add(channels[i], channels[3], channels[i]);

		merge(channels, 3, image);
	};
}

#endif