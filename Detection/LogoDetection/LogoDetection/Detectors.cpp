#include "Detectors.h"
#include "GeneralTransforms.h"
#include "Geometry.h"

#include <unordered_set>

/*members of Detector */

Detector::Detector()
	: working(false)
{	};

bool Detector::isWorking() const {
	return working;
};

String Detector::getName() const {
	return name;
};

/*members of SiftDetector */

int SiftDetector::detections = 0;

SiftDetector::SiftDetector(String n, int minHess)  {
	minHessian = minHess;
	name = n;
};

// check, convert to grayscale, calculate keypoints and descriptors
void SiftDetector::process(Mat &image) {

	if (!image.data)
		return;
	
	obj_corners.resize(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(image.cols, 0);
	obj_corners[2] = cvPoint(image.cols, image.rows);
	obj_corners[3] = cvPoint(0, image.rows);
	
	cvtColor(image, image, CV_BGR2GRAY);

	// SiftFeatureDetector have an option of contrast threshold but this worked out better
	image.convertTo(image, -1, 1.5, 0);

	try {

		SiftFeatureDetector detector(minHessian, 3);
		detector.detect(image, keypoints);

		if (keypoints.size() < MIN_POINTS)
			return;

		SiftDescriptorExtractor extractor;
		extractor.compute(image, keypoints, descriptors);

		working = true;
	}
	catch (Exception e) {
		logg::err.open("errlog.log", std::ofstream::app);
		logg::err
			<< "process()" << endl
			<< name << endl 
			<< e.msg << endl 
			<< "img type: " << image.type() << endl;
	}
};

// find matches in two precalculated sets of keypoints and descriptors
void SiftDetector::match(const SiftDetector sd_scene, Mat &img_scene) const {

	if (!working || !sd_scene.isWorking())
		return;

	std::vector< DMatch > matches;
	BFMatcher matcher;
	matcher.match(descriptors, sd_scene.descriptors, matches);

	double max_dist = 0, min_dist = 100;

	for (int i = 0; i < descriptors.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors.rows; i++) {
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	if (good_matches.size() < MIN_MATCHES)
		return;

	unordered_set<int> used_matches;

	// search for the same object while there are left any unused good_matches 
	while ((good_matches.size() - used_matches.size()) > MIN_MATCHES) {

		std::vector<Point2f> obj_points;
		std::vector<Point2f> scn_points;

		// get the keypoints from the good matches
		for (int i = 0; i < good_matches.size(); i++) {
			if (used_matches.find(i) == used_matches.end()) {
				obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
				scn_points.push_back(sd_scene.keypoints[good_matches[i].trainIdx].pt);
			}
		}

		Mat H = findHomography(obj_points, scn_points, CV_RANSAC);

		std::vector<Point2f> scn_corners(4);
		perspectiveTransform(obj_corners, scn_corners, H);

		// check whether result looks realistic
		if (!geom::checkQuadrangle(scn_corners))
			return;

		SiftDetector::detections++;
		logg::tout << "found: " << name << endl;

		for (int i = 0; i < 4; i++)
			line(img_scene, scn_corners[i], scn_corners[(i + 1) % 4], Scalar(0, 255, 0), 3);

		putText(img_scene, to_upper_copy<std::string>(name),
			geom::centroid(scn_corners) - Point2f(30,0), FONT_HERSHEY_DUPLEX, 0.5,
			Scalar(255, 255, 255), 1, 1, 0);

		putText(img_scene, to_upper_copy<std::string>(name),
			geom::centroid(scn_corners) - Point2f(30, 15), FONT_HERSHEY_DUPLEX, 0.5,
			Scalar(0, 0, 0), 1, 1, 0);

		// remember points of the found object

		geom::Quadrangle quad(scn_corners);

		for (int i = 0; i < good_matches.size(); i++) {
			if (quad.surrounds(sd_scene.keypoints[good_matches[i].trainIdx].pt))
				used_matches.insert(i);
		}
	}
}
