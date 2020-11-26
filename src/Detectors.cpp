#include <unordered_set>
#include "opencv2/xfeatures2d.hpp"

#include "Detectors.h"
#include "GeneralTransforms.h"
#include "Geometry.h"

#include <boost/algorithm/string.hpp>


/*members of Detector */

Detector::Detector()
	: working(false)
{	};

bool Detector::isWorking() const {
	return working;
};

std::string Detector::getName() const {
	return name;
};

/*members of SiftDetector */

//int SiftDetector::detections = 0;

SiftDetector::SiftDetector(const std::string & n, int minHess)  {

	minHessian = minHess;
	name = n;
//	sift = cv::ORB::create();
	sift = cv::xfeatures2d::SIFT::create();
};

void pretty_put_line(const cv::Mat & image, std::string line, const cv::Point & pos)
{
	auto font_id = cv::FONT_HERSHEY_PLAIN;
	double font_scale = 1;

	int thickness = 3;
	cv::putText(image, line, pos, font_id, font_scale, {0, 0, 0, 0}, 3);

	thickness = 1;
	cv::putText(image, line, pos, font_id, font_scale, {255, 255, 125}, 1);
}

// check, convert to grayscale, calculate keypoints and descriptors
void SiftDetector::process(cv::Mat image) {

	if (!image.data)
		return;
	
	obj_corners.resize(4);

	obj_corners[0] = cv::Point(0, 0);
	obj_corners[1] = cv::Point(image.cols, 0);
	obj_corners[2] = cv::Point(image.cols, image.rows);
	obj_corners[3] = cv::Point(0, image.rows);
	assert(!image.empty());
//	log_state << image.type() << std::endl;
	cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	// SiftFeatureDetector have an option of contrast threshold but this worked out better
	image.convertTo(image, -1, 1.5, 0);

//	SiftFeatureDetector detector(minHessian, 3);
	sift->detect(image, keypoints);

//	detector.detect(image, keypoints);

	if (keypoints.size() < MIN_POINTS)
		return;

//	Mat descriptors_1, descriptors_2;
	sift->compute(image, keypoints, descriptors);

//	SiftDescriptorExtractor extractor;
//	extractor.compute(image, keypoints, descriptors);

	working = true;
};

// find matches in two precalculated sets of keypoints and descriptors
void SiftDetector::match(const SiftDetector sd_scene, const cv::Mat &img_scene) const {

	if (!working || !sd_scene.isWorking())
		return;

	std::vector<cv::DMatch> matches;
	cv::BFMatcher matcher;
	matcher.match(descriptors, sd_scene.descriptors, matches);

	double max_dist = 0, min_dist = 100;
	for (int i = 0; i < descriptors.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < descriptors.rows; i++) {
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	if (good_matches.size() < MIN_MATCHES)
		return;

	std::unordered_set<int> used_matches;

	// search for the same object while there are left any unused good_matches 
	while ((good_matches.size() - used_matches.size()) > MIN_MATCHES) {

		std::vector<cv::Point2f> obj_points;
		std::vector<cv::Point2f> scn_points;

		// get the keypoints from the good matches
		for (int i = 0; i < good_matches.size(); i++) {
			if (used_matches.find(i) == used_matches.end()) {
				obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
				scn_points.push_back(sd_scene.keypoints[good_matches[i].trainIdx].pt);
			}
		}

		cv::Mat homography = cv::findHomography(obj_points, scn_points, cv::RANSAC);

		std::vector<cv::Point2f> scn_corners(4);
//		log_state << obj_corners.size() << std::endl;
//		log_state << homography.size() << std::endl;

		if (homography.empty()){
			break;
		}

		cv::perspectiveTransform(obj_corners, scn_corners, homography);

		// check whether result looks realistic
		if (!geom::checkQuadrangle(scn_corners))
			return;

		geom::Quadrangle quad(scn_corners);

		for (int i = 0; i < good_matches.size(); i++) {
			if (quad.surrounds(sd_scene.keypoints[good_matches[i].trainIdx].pt))
				used_matches.insert(i);
		}

//		SiftDetector::detections++;


		for (auto & p : scn_corners){
			p.x = std::max<float>(p.x, 0);
			p.y = std::max<float>(p.y, 0);
			p.x = std::min<float>(p.x, img_scene.size().width);
			p.y = std::min<float>(p.y, img_scene.size().height);
		}

		log_state << "found: " << name << std::endl;

		for (int i = 0; i < scn_corners.size(); i++){
			auto p1 = scn_corners[i];
			auto p2 = scn_corners[(i + 1) % scn_corners.size()];
			line(img_scene, p1, p2, cv::Scalar(0, 255, 0), 3);
		}

//		putText(img_scene, boost::to_upper_copy<std::string>(name),
//			geom::centroid(scn_corners) - cv::Point2f(30,0), cv::FONT_HERSHEY_DUPLEX, 0.5,
//			    cv::Scalar(255, 255, 255), 1, 1, 0);
//
//		putText(img_scene, boost::to_upper_copy<std::string>(name),
//			geom::centroid(scn_corners) - cv::Point2f(30, 15), cv::FONT_HERSHEY_DUPLEX, 0.5,
//			    cv::Scalar(0, 0, 0), 1, 1, 0);

		pretty_put_line(
				img_scene,
				boost::to_upper_copy<std::string>(name),
				geom::centroid(scn_corners) - cv::Point2f(30,0)
				);

		// remember points of the found object

		cv::imshow("scene", img_scene);
		cv::waitKey(10);
	}
}
