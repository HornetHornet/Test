#pragma once

#ifndef GENERALTRAMSFORSM_H
#define GENERALTRAMSFORSM_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

//resize the Mat down if it's too big
static void shrinkTo(Mat &image, double size) {
	if (max(image.rows, image.cols) > size) {
		double resizeFactor = min(size / image.rows, size / image.cols);
		resize(image, image, Size(), resizeFactor, resizeFactor, cv::INTER_CUBIC);
	}
};

//apply bilateralFilter with kernel = 7
static void filterIt(Mat &image) {
	static const int filteringKernel = 9;
	Mat imCopy = image.clone();
	bilateralFilter(image, imCopy, filteringKernel, filteringKernel * 2, filteringKernel / 2);
	// 	image = imCopy.clone(); medianBlur(image, image, 3);
}

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


#endif