#pragma once

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Logger.h"


#define MIN_POINTS 18 	// with less than that points object would not be found anyway
#define MIN_MATCHES 9   // less then this matcing points will cause false alarms


#ifndef DETECTORS_H
#define DETECTORS_H

class Detector {
protected:
	std::string object_id;
	bool working = false;

	Detector();

public:
	std::string getName() const;
	bool isWorking() const;
};

class SiftDetector : public Detector {
	cv::Ptr<cv::Feature2D> features;

	std::vector<cv::Point2f> obj_corners;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

public:
	SiftDetector(const std::string &object_id, int MinHess);

	void process(cv::Mat image);
	void match(const SiftDetector & sd_scene, const cv::Mat &img_scene) const;

//	static int SiftDetector::detections;
};

#endif