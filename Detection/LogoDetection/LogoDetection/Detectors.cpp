#include "Detectors.h"
#include "GeneralTransforms.h"
#include "Geometry.h"

/*members of Logger*/

ofstream Logger::clocklog("clock.log", std::ofstream::app);
ofstream Logger::errlog("err.log", std::ofstream::app);

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

FlannBasedMatcher SiftDetector::matcher;
SiftFeatureDetector SiftDetector::detector(400);
SiftDescriptorExtractor SiftDetector::extractor;

SiftDetector::SiftDetector(String n)  {
	name = n;
};

// check, convert to grayscale, resize, filter, calculate keypoints and descriptors
void SiftDetector::process(Mat img, bool isScene) {
	cout << "	processing.. " << endl;
	image = img.clone();

	if (image.empty())
		return;

	//trnsf::reduce(image, 20);
	trnsf::resize(image, isScene ? 780 : 256);

	if (image.channels() == 4) 
		trnsf::alpha_to_white(image);

	cvtColor(image, image, CV_BGR2GRAY);

	try {
		trnsf::filter(image);

		cout << "	keypoints..   ";
		SiftDetector::detector.detect(image, keypoints);
		cout << keypoints.size() << endl;

		if (keypoints.size() < 3)
			return;

		cout << "	descriptors.. ";
		SiftDetector::extractor.compute(image, keypoints, descriptors);
		cout << descriptors.size() << endl << endl;

		working = true;
	}
	catch (Exception e) {
		Logger::errlog << e.msg << endl << "img type: " << image.type() << endl;
	}

	return;
};

// find matches in two precalculated sets of keypoints and descriptors
bool SiftDetector::match(SiftDetector sd_scene, Mat &img_scene) {

	cout << endl 
		<< " matching  " << sd_scene.name << endl
		<< "      and  " << name << endl;

	if (!working || !sd_scene.isWorking())
		return false;

	std::vector< DMatch > matches;
	ofstream clocklog("clock.log", std::ofstream::app);
	time_t start = clock();

	SiftDetector::matcher.match(descriptors, sd_scene.descriptors, matches);

	Logger::clocklog << endl << name << " and " << sd_scene.name << endl
			<< "match() took: " << clock() - start << endl
			<< "descriptors.size(): " << descriptors.size() << endl
			<< "Sift_scne.descriptors.size(): " << sd_scene.descriptors.size() << endl;

	double max_dist = 0; double min_dist = 100;

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

	if (good_matches.size() < 4)
		return false;

	Mat img_matches;

	drawMatches(image, keypoints, sd_scene.image, sd_scene.keypoints,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//imshow("Good Matches & Object detection", img_matches);

	std::vector<Point2f> obj_points;
	std::vector<Point2f> scn_points;

	//-- Get the keypoints from the good matches
	for (int i = 0; i < good_matches.size(); i++) {
		//if (i % 2) continue;
		obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
		scn_points.push_back(sd_scene.keypoints[good_matches[i].trainIdx].pt);
	}

	try {

		start = clock();

		Mat H = findHomography(obj_points, scn_points, CV_RANSAC);

		Logger::clocklog << "findHomography() took: " << clock() - start << endl
			<< "obj_points: " << obj_points.size() << endl
			<< "scn_points: " << scn_points.size() << endl << endl;
				
		std::vector<Point2f> obj_corners(4);
		std::vector<Point2f> scn_corners(4);

		obj_corners[0] = cvPoint(0, 0); 
		obj_corners[1] = cvPoint(image.cols, 0);
		obj_corners[2] = cvPoint(image.cols, image.rows); 
		obj_corners[3] = cvPoint(0, image.rows);

		perspectiveTransform(obj_corners, scn_corners, H);

		//for (int i = 0; i < 4; i++) {
		//	line(img_matches,
		//		scn_corners[i] + Point2f(image.cols, 0),
		//		scn_corners[(i + 1) % 4] + Point2f(image.cols, 0),
		//		Scalar(0, 0, 255), 2
		//		);
		//}
		//		
		//for (int i = 0; i < 4; i++)
		//	line(img_scene, scn_corners[i], scn_corners[(i + 1) % 4], Scalar(0, 0, 255), 1);

		//imshow("Good Matches & Object detection", img_matches); cv::waitKey(50);

		if (geom::checkQuadrangle(scn_corners)) {
			clocklog << "matched";

			for (int i = 0; i < 4; i++)
				line(img_scene, scn_corners[i], scn_corners[(i + 1) % 4], Scalar(0, 255, 0), 3);

			Point2f center;

			putText(img_scene, to_upper_copy<std::string>(name), 
				Point(scn_corners[0].x, scn_corners[0].y - 5), FONT_HERSHEY_DUPLEX, 0.5,
				Scalar(255, 255, 0), 1, 1, 0);

			//cv::waitKey(0);

			return true;
		}
	}
	catch (Exception e) {			
		Logger::errlog << name << " and " << sd_scene.name << endl
			<< "obj_points " << obj_points.size() << endl
			<< "scn_points " << scn_points.size() << endl << endl;
	}
	return false;
}
