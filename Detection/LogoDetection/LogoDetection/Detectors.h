#pragma once

#include "stdafx.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#ifndef DETECTORS_H
#define DETECTORS_H

class Detector {
protected:
	String name;
	bool working = false;
	Detector();
public:
	String getName() const;
	bool isWorking() const;
};


	Mat image;

	SurfFeatureDetector detector = SurfFeatureDetector(400);
	SurfDescriptorExtractor extractor;

	std::vector<KeyPoint> keypoints;
	Mat descriptors;

public:
	SurfDetector(String n);
	void process(Mat image, bool isScene = false);
	bool match(SurfDetector surf_scene, Mat &img_scene);
};

#endif