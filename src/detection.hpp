#pragma once

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "logger.hpp"


#define MIN_POINTS 13 // with less than that points object would not be found anyway
#define MIN_MATCHES 9   // less then this matcing points will cause false alarms


#ifndef DETECTORS_H
#define DETECTORS_H

class Detector
{
protected:
	Detector(const std::string & obj_id);
	virtual ~Detector() = default;
public:
	const std::string object_id;
};

class KeyPointFeatureDetector : public Detector
{
	cv::Ptr<cv::Feature2D> features;
	std::vector<cv::Point2d> obj_corners;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

public:
	static cv::Mat prepare_image(const cv::Mat & img);

	KeyPointFeatureDetector(const std::string &obj_id);
	virtual ~KeyPointFeatureDetector() = default;
	bool process(cv::Mat image);

	std::vector<std::vector<cv::Point2d>> match(
			const KeyPointFeatureDetector &sd_scene) const;

//	static int KeyPointFeatureDetector::detections;
};

#endif