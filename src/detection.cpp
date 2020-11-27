#include <unordered_set>
#include <opencv2/xfeatures2d.hpp>
#include <boost/algorithm/string.hpp>
#include "detection.hpp"
#include "img-utils.hpp"
#include "geom-utils.hpp"


Detector::Detector(const std::string & obj_id) : object_id(obj_id)
{};


//int KeyPointFeatureDetector::detections = 0;

KeyPointFeatureDetector::KeyPointFeatureDetector(const std::string &obj_id, int minHess)
: Detector(obj_id)
{
	features = cv::xfeatures2d::SIFT::create(minHess, 3);
};

cv::Mat KeyPointFeatureDetector::prepare_image(const cv::Mat & src)
{
	expect(!src.empty());
	cv::Mat dst;
	cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);

	// SiftFeatureDetector have an option of contrast threshold but this worked out better
	dst.convertTo(dst, -1, 1.5, 0);
	return dst;
}


// check, convert to grayscale, calculate key points and descriptors
bool KeyPointFeatureDetector::process(cv::Mat image)
{
	expect(!image.empty());

	obj_corners = {
			cv::Point(0, 0),
			cv::Point(image.cols, 0),
			cv::Point(image.cols, image.rows),
			cv::Point(0, image.rows),
	};

	features->detect(image, keypoints);

	if (keypoints.size() < MIN_POINTS){
		log_err
		<< "Failed to find at least " << MIN_POINTS << " points on " << object_id << ", " <<
		" only " << keypoints.size() << " "
		<< "Won't search for it" << std::endl;
		return false;
	}

	features->compute(image, keypoints, descriptors);
	return true;
};

// find matches in two precalculated sets of key points and descriptors
std::vector<std::vector<cv::Point2d>> KeyPointFeatureDetector::match(
		const KeyPointFeatureDetector &sd_scene) const
{
	std::vector<cv::DMatch> matches;
	cv::BFMatcher matcher;
	matcher.match(descriptors, sd_scene.descriptors, matches);

	double max_dist = 0, min_dist = 100;
	for (int i = 0; i < descriptors.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < descriptors.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	if (good_matches.size() < MIN_MATCHES)
		return {};

	std::unordered_set<int> used_matches;

	std::vector<std::vector<cv::Point2d>> ans_boxes;

	// search for the same object while there are left any unused good_matches 
	while ((good_matches.size() - used_matches.size()) > MIN_MATCHES)
	{

		std::vector<cv::Point2d> obj_points;
		std::vector<cv::Point2d> scn_points;

		// get the key points from the good matches
		for (int i = 0; i < good_matches.size(); i++)
		{
			if (used_matches.find(i) == used_matches.end())
			{
				obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
				scn_points.push_back(sd_scene.keypoints[good_matches[i].trainIdx].pt);
			}
		}

		cv::Mat homography = cv::findHomography(obj_points, scn_points, cv::RANSAC);
//		log_state << homography.size() << std::endl;
		if (homography.empty())
		{
			break;
		}

		std::vector<cv::Point2d> scn_corners(4);
//		log_state << obj_corners.size() << std::endl;
		cv::perspectiveTransform(obj_corners, scn_corners, homography);

		// check whether result looks realistic
		if (!geom::checkQuadrangle(scn_corners))
			break;

		geom::Quadrangle quad(scn_corners);

		for (int i = 0; i < good_matches.size(); i++)
		{
			if (quad.surrounds(sd_scene.keypoints[good_matches[i].trainIdx].pt))
				used_matches.insert(i);
		}

		log_state << "found: " << object_id << std::endl;

		ans_boxes.push_back(scn_corners);
	}

	return ans_boxes;
}
