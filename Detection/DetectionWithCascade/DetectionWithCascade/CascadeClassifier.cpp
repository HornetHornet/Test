#include "stdafx.h"
#include "CascadeWrapper.h"

CascadeWrapper::CascadeWrapper() {};

void CascadeWrapper::setAndLoad(const path &pathToCascade) {
	cascadePath = pathToCascade.string();
	targetName = pathToCascade.stem().string();
	working = classifier.load(cascadePath);
	cout << (working ? "loaded " : "ERROR: could not load ") << cascadePath << endl;
};

void CascadeWrapper::detectAndDisplay(Mat &frame) {

	if (!working) {
		cout << "ERROR: " << cascadePath << " unavailale" << endl;
		return;
	}

	std::vector<Rect> targets;
	Mat imGray;

	cvtColor(frame, imGray, COLOR_BGR2GRAY);
	equalizeHist(imGray, imGray);

	cout << "detecting " << targetName << endl;

	classifier.detectMultiScale(imGray, targets, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < targets.size(); i++)
	{
		cout << "got " << targetName << endl;

		Rect rect(targets[i].x, targets[i].y, targets[i].width, targets[i].height);
		rectangle(frame, rect, Scalar(255, 0, 255), 2, 8, 0);

		putText(frame, targetName, Point(targets[i].x + targets[i].width, targets[i].y), 0, 0.4, Scalar(255, 255, 0), 1, 1, 0);

	}
}
