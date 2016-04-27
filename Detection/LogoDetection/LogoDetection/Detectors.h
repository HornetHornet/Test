#pragma once

#include "stdafx.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#define MIN_POINTS 60
#define MIN_MATCHES 9

#ifndef DETECTORS_H
#define DETECTORS_H

class Detector : public logg {
protected:
	String name;
	bool working = false;

	Detector();

public:
	String getName() const;
	bool isWorking() const;
};

class SiftDetector : public Detector {

	int minHessian;

	std::vector<Point2f> obj_corners;
	std::vector<KeyPoint> keypoints;
	Mat descriptors;

public:
	SiftDetector(String n, int MinHess);

	void process(Mat &image);
	void match(const SiftDetector sd_scene, Mat &img_scene);

	static int SiftDetector::detections;
};

#endif