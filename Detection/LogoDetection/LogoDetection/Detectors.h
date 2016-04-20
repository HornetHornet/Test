#pragma once

#include "stdafx.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#ifndef DETECTORS_H
#define DETECTORS_H

class Detector {
protected:
	String name;
	bool working = false;

	Detector();
public:
	bool isWorking() const;
};

class SurfDetector : public Detector {

	Mat image;

	SurfFeatureDetector detector = SurfFeatureDetector(400);
	SurfDescriptorExtractor extractor;

	vector<KeyPoint> keypoints;
	Mat descriptors;

public:
	SurfDetector(String n);
	void process(Mat image, bool isScene = false);
	void match(SurfDetector surf_scene);
};

#endif