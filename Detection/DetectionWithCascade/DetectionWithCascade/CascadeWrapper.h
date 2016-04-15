#pragma once

#include "stdafx.h"

#ifndef CASCADEWRAPPER_H
#define CASCADEWRAPPER_H

class CascadeWrapper {
	String cascadePath;
	String targetName;
	CascadeClassifier classifier;
	bool working = false;

public:

	CascadeWrapper();
	void setAndLoad(const path &pathToCascade);
	void detectAndDisplay(Mat &frame);
};

#endif