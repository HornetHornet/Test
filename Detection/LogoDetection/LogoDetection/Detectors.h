#pragma once

#include "stdafx.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#ifndef DETECTORS_H
#define DETECTORS_H

class Logger {
public:
	static ofstream clocklog;
	static ofstream errlog;
};

class Detector : public Logger {
protected:
	String name;
	bool working = false;
	Detector();

public:
	String getName() const;
	bool isWorking() const;
};

class SiftDetector : public Detector {

	static FlannBasedMatcher matcher;
	static SiftFeatureDetector detector;
	static SiftDescriptorExtractor extractor;

	Mat image;
	std::vector<KeyPoint> keypoints;
	Mat descriptors;

public:
	SiftDetector(String n);
	void process(Mat image, bool isScene = false);
	bool match(SiftDetector sd_scene, Mat &img_scene);
};


#endif