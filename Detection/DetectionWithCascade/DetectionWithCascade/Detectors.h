#pragma once

#include "stdafx.h"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp


#ifndef DETECTORS_H
#define DETECTORS_H

class Detector {
protected:
	String resourcePath;
	String targetName;
	bool working = false;
	Detector();
public:
	bool isWorking() const;
	virtual void setAndLoad(const path &pathToResource) {};
	virtual void detectAndDisplay(Mat &frame) {};
};


class CascadeDetector : public Detector {

	CascadeClassifier classifier;
public:
	CascadeDetector();
	virtual void setAndLoad(const path &pathToResource);
	virtual void detectAndDisplay(Mat &frame);
};


class SurfDetector : public Detector {

	Mat referenceImage;

	static const int minHessian = 400;
	static const SurfFeatureDetector detector(minHessian);

public:
	SurfDetector();
	virtual void setAndLoad(const path &pathToResource);
	virtual void detectAndDisplay(Mat &frame);
};

#endif