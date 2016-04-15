#include "CascadeDetector.h"

CascadeDetector::CascadeDetector() {};

void CascadeDetector::setAndLoad(const path &pathToResource) {
	resourcePath = pathToResource.string();
	targetName = pathToResource.stem().string();
	working = classifier.load(resourcePath);
	cout << (working ? "loaded " : "ERROR: could not load ") << resourcePath << endl;
};

void CascadeDetector::detectAndDisplay(Mat &frame) {

	if (!working) {
		cout << "ERROR: " << resourcePath << " unavailale" << endl;
		return;
	}

	std::vector<Rect> targets;
	Mat imGray;

	cvtColor(frame, imGray, COLOR_BGR2GRAY);
	equalizeHist(imGray, imGray);

	cout << "	detecting " << targetName << endl;

	classifier.detectMultiScale(imGray, targets, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < targets.size(); i++) {
		cout << "		got " << targetName << endl;

		Rect rect(targets[i].x, targets[i].y, targets[i].width, targets[i].height);
		rectangle(frame, rect, Scalar(255, 0, 255), 2, 8, 0);

		putText(frame, to_upper_copy<std::string>(targetName), Point(targets[i].x, targets[i].y), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 255, 0), 1, 1, 0);
	}
}