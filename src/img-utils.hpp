#pragma once

#ifndef GENERALTRAMSFORSM_H
#define GENERALTRAMSFORSM_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "logger.hpp"

//resize so that the max dimention be equal to "size"
static void preciseResize(cv::Mat &image, double size)
{
	double resizeFactor = std::min(size / image.rows, size / image.cols);
	cv::resize(image, image, cv::Size(), resizeFactor, resizeFactor, cv::INTER_CUBIC);
};

//resize down if one of image dimensions is more than "size"
static void resizeDown(cv::Mat &image, double size)
{
	if (std::max(image.rows, image.cols) > size)
		preciseResize(image, size);
};

//resize up if image dimensions is less than "size"
static void resizeUp(cv::Mat &image, double size)
{
	if (std::max(image.rows, image.cols) < size)
		preciseResize(image, size);
};

//replace transparent with white
static void makeOpaque(cv::Mat &image)
{
	expect(image.channels() == 4);
	cv::Mat channels[4];

	split(image, channels);
	bitwise_not(channels[3], channels[3]);
	//channels[3].convertTo(channels[3], -1, 1, -128);

	for (size_t i = 0; i < 3; i++)
		add(channels[i], channels[3], channels[i]);

	merge(channels, 3, image);
};

static void pretty_put_text_line(
		const cv::Mat &image,
		const std::string & line,
		const cv::Point &pos,
		const cv::Scalar &clr = {255, 255, 125}
)
{
	auto font_id = cv::FONT_HERSHEY_PLAIN;
	double font_scale = 1;
	int thickness = 3;
	cv::putText(image, line, pos, font_id, font_scale, {0, 0, 0}, thickness);

	thickness = 1;
	cv::putText(image, line, pos, font_id, font_scale, clr, thickness);
}


static cv::Scalar makeRandColor()
{
	cv::Scalar clr;

	double maxc = 0;
	double minc = 255;

	for (int i = 0; i < 3; ++i)
	{
		clr[i] = rand() % 256;
		maxc = std::max<double>(maxc, clr[i]);
		minc = std::min<double>(minc, clr[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
//		clr[i] = 1. * (clr[i] - minc) / (maxc - minc) * 255;
		clr[i] = 1. * clr[i] / maxc * 255;
	}

	return clr;
}


static void fitPoly(std::vector<cv::Point2d> & poly, const cv::Size & img_size)
{
	for (auto &point : poly)
	{
		point.x = std::max<double>(0, std::min<double>(point.x, img_size.width));
		point.y = std::max<double>(0, std::min<double>(point.y, img_size.height));
	}
}


static void drawPoly(cv::Mat & img, const std::vector<cv::Point2d> & poly, const cv::Scalar & clr)
{
	for (int i = 0; i < poly.size(); i++)
	{
		auto p1 = poly[i];
		auto p2 = poly[(i + 1) % poly.size()];
		line(img, p1, p2, clr, 3);
	}
}

#endif