#include "Detectors.h"
#include "GeneralTransforms.h"

/*members of Detector */

Detector::Detector()
	: working(false)
{	};

bool Detector::isWorking() const {
	return working;
};

/*members of SurfDetector */

	SurfDetector::SurfDetector(String n)  {
		name = n;
	};

	void SurfDetector::process(Mat img, bool isScene) {

		image = img.clone();

		working = !image.empty();

		if (working) {
			cvtColor(image, image, CV_BGR2GRAY);

			shrinkTo(image, isScene ? 720 : 256);
			filterIt(image);

			cout << "	detecting keypoints..   ";
			detector.detect(image, keypoints);
			cout << keypoints.size() << endl;

			cout << "	computing descriptors.. ";
			extractor.compute(image, keypoints, descriptors);
			cout << descriptors.size() << endl << endl;
		}
		else 	
			cout << " Warning: " << name << " is not funtional" << endl << endl;

	};

	void SurfDetector::match(SurfDetector scene) {

		cout << endl << " matching " << name << " and " << scene.name << endl;

		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(descriptors, scene.descriptors, matches);

		double max_dist = 0; double min_dist = 100;

		for (int i = 0; i < descriptors.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		printf("	-- Max dist : %f \n", max_dist);
		printf("	-- Min dist : %f \n", min_dist);

		std::vector< DMatch > good_matches;
		for (int i = 0; i < descriptors.rows; i++) {
			if (matches[i].distance < 3 * min_dist) {
				good_matches.push_back(matches[i]);
			}
		}

		Mat img_matches;

		drawMatches(image, keypoints, scene.image, scene.keypoints,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		std::vector<Point2f> obj_points;
		std::vector<Point2f> scene_points;
		for (int i = 0; i < good_matches.size(); i++) {
			//-- Get the keypoints from the good matches
			obj_points.push_back(keypoints[good_matches[i].queryIdx].pt);
			scene_points.push_back(scene.keypoints[good_matches[i].trainIdx].pt);
		}

		imshow("Good Matches & Object detection", img_matches);

		try {
			Mat H = findHomography(obj_points, scene_points, CV_RANSAC);

			cout << "	Geting the corners from the " <<  name << endl;
			std::vector<Point2f> obj_corners(4);
			obj_corners[0] = cvPoint(0, 0); 
			obj_corners[1] = cvPoint(image.cols, 0);
			obj_corners[2] = cvPoint(image.cols, image.rows); 
			obj_corners[3] = cvPoint(0, image.rows);

			std::vector<Point2f> scene_corners(4);
			perspectiveTransform(obj_corners, scene_corners, H);

			line(img_matches, scene_corners[0] + Point2f(image.cols, 0), 
				scene_corners[1] + Point2f(image.cols, 0), Scalar(255, 0, 0), 4);

			line(img_matches, scene_corners[1] + Point2f(image.cols, 0), 
				scene_corners[2] + Point2f(image.cols, 0), Scalar(0, 255, 0), 4);

			line(img_matches, scene_corners[2] + Point2f(image.cols, 0), 
				scene_corners[3] + Point2f(image.cols, 0), Scalar(0, 0, 255), 4);

			line(img_matches, scene_corners[3] + Point2f(image.cols, 0), 
				scene_corners[0] + Point2f(image.cols, 0), Scalar(255, 255, 0), 4);

			imshow("Good Matches & Object detection", img_matches);
		}
		catch (Exception e) {
			cout << endl << e.msg << endl;
		}

		waitKey(50);
	}
