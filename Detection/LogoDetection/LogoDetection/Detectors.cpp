#include "Detectors.h"
#include "GeneralTransforms.h"
#include "Geometry.h"


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

SiftDetector::SiftDetector(String n)  {
	name = n;
};

// check, convert to grayscale, resize, filter, calculate keypoints and descriptors
void SiftDetector::process(Mat img) {

	Mat image = img.clone();

	obj_corners.resize(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(image.cols, 0);
	obj_corners[2] = cvPoint(image.cols, image.rows);
	obj_corners[3] = cvPoint(0, image.rows);
	
	if (image.channels() == 4)
		trnsf::makeOpaque(image);

	cvtColor(image, image, CV_BGR2GRAY);
	image.convertTo(image, -1, 1.5, 0);

	try {

		SiftFeatureDetector detector(400);
		detector.detect(image, keypoints);

		if (keypoints.size() < 3)
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

	return;
};

// find matches in two precalculated sets of keypoints and descriptors
void SiftDetector::match(SiftDetector sd_scene, Mat &img_scene) {

	if (!working || !sd_scene.isWorking())
		return;

	std::vector< DMatch > matches;
	FlannBasedMatcher matcher;

	matcher.match(descriptors, sd_scene.descriptors, matches);

	double max_dist = 0, min_dist = 100;

	for (int i = 0; i < descriptors.rows; i++)	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors.rows; i++) {
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	if (good_matches.size() < 9)
		return;

	std::vector<Point2f> obj_points;
	std::vector<Point2f> scn_points;

	//-- Get the keypoints from the good matches
	for (int i = 0; i < good_matches.size(); i++) {
		obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
		scn_points.push_back(sd_scene.keypoints[good_matches[i].trainIdx].pt);
	}

	try {

		Mat H = findHomography(obj_points, scn_points, CV_RANSAC);
		std::vector<Point2f> scn_corners(4);

		perspectiveTransform(obj_corners, scn_corners, H);

		if (geom::checkQuadrangle(scn_corners)) {

			for (int i = 0; i < 4; i++)
				line(img_scene, scn_corners[i], scn_corners[(i + 1) % 4], Scalar(0, 255, 0), 3);

			putText(img_scene, to_upper_copy<std::string>(name), 
				Point(scn_corners[0].x + 10, scn_corners[0].y + 20), FONT_HERSHEY_DUPLEX, 0.5,
				Scalar(255, 255, 255), 1, 1, 0);

			putText(img_scene, to_upper_copy<std::string>(name),
				Point(scn_corners[0].x + 10, scn_corners[0].y + 35), FONT_HERSHEY_DUPLEX, 0.5,
				Scalar(0, 0, 0), 1, 1, 0);

			SiftDetector::detections++;
			logg::tout << "found: " << name << endl;
		}
	}
	catch (Exception e) {
		logg::err.open("errlog.log", std::ofstream::app);
		logg::err
			<< name << " and " << sd_scene.name << endl
			<< "obj_points " << obj_points.size() << endl
			<< "scn_points " << scn_points.size() << endl << endl;
	}
}
