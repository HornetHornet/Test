#pragma once

#ifndef GENERALTRAMSFORSM_H
#define GENERALTRAMSFORSM_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

inline void shrinkTo(Mat &image, double size) {

	if (max(image.rows, image.cols) > size) {
		double resizeFactor = min(size / image.rows, size / image.cols);
		resize(image, image, Size(), resizeFactor, resizeFactor, cv::INTER_CUBIC);
	}
};

inline void filterIt(Mat &image) {
	static const int filteringKernel = 7;

	Mat imCopy = image.clone();
	bilateralFilter(image, imCopy, filteringKernel, filteringKernel * 2, filteringKernel / 2);
	/*image = imCopy.clone();
	medianBlur(image, image, 3);*/
}

#endif