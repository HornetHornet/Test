#include "Detectors.h"

/*members of Detector */

Detector::Detector() { };

bool Detector::isWorking() const {
	return working;
};

/*members of CascadeDetector */

CascadeDetector::CascadeDetector() {};

void CascadeDetector::setAndLoad(const path &pathToResource) {
	resourcePath = pathToResource.string();
	targetName = pathToResource.stem().string();
	working = classifier.load(resourcePath);
	cout << (working ? "loaded " : "ERROR: could not load ") << resourcePath << endl;
};

void CascadeDetector::detectAndDisplay(Mat &frame) {

	if (!working) {
		cout << "ERROR: " << resourcePath << " is unavailable" << endl;
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

/*members of SurfDetector */

	SurfDetector::SurfDetector() { };

	void SurfDetector::setAndLoad(const path &pathToResource) {

		referenceImage = imread(pathToResource.string(), 1);

		//working = !referenceImage.empty());
		
	};

	void SurfDetector::detectAndDisplay(Mat &frame) {
		//std::vector<KeyPoint> keypoints_object, keypoints_scene;

		//detector.detect(img_object, keypoints_object);
		//detector.detect(img_scene, keypoints_scene);

		////-- Step 2: Calculate descriptors (feature vectors)
		//SurfDescriptorExtractor extractor;

		//Mat descriptors_object, descriptors_scene;

		//extractor.compute(img_object, keypoints_object, descriptors_object);
		//extractor.compute(img_scene, keypoints_scene, descriptors_scene);

		////-- Step 3: Matching descriptor vectors using FLANN matcher
		//FlannBasedMatcher matcher;
		//std::vector< DMatch > matches;
		//matcher.match(descriptors_object, descriptors_scene, matches);

		//double max_dist = 0; double min_dist = 100;

		////-- Quick calculation of max and min distances between keypoints
		//for (int i = 0; i < descriptors_object.rows; i++)
		//{
		//	double dist = matches[i].distance;
		//	if (dist < min_dist) min_dist = dist;
		//	if (dist > max_dist) max_dist = dist;
		//}

		//printf("-- Max dist : %f \n", max_dist);
		//printf("-- Min dist : %f \n", min_dist);

		////-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		//std::vector< DMatch > good_matches;

		//for (int i = 0; i < descriptors_object.rows; i++)
		//{
		//	if (matches[i].distance < 3 * min_dist)
		//	{
		//		good_matches.push_back(matches[i]);
		//	}
		//}

		//Mat img_matches;
		//drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		//	good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		////-- Localize the object
		//std::vector<Point2f> obj;
		//std::vector<Point2f> scene;

		//for (int i = 0; i < good_matches.size(); i++)
		//{
		//	//-- Get the keypoints from the good matches
		//	obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		//	scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
		//}

		//Mat H = findHomography(obj, scene, CV_RANSAC);

		////-- Get the corners from the image_1 ( the object to be "detected" )
		//std::vector<Point2f> obj_corners(4);
		//obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
		//obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
		//std::vector<Point2f> scene_corners(4);

		//perspectiveTransform(obj_corners, scene_corners, H);
	};