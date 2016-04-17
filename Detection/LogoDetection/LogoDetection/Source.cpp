#include "stdafx.h"

#include <opencv2/highgui.hpp>

#include "Detectors.h"
#include "GetFiles.h"

String window_name = "Detection";

//template <class T> ostream &operator << (ostream &stream, const std::vector <T> &vect) {
//	for each (T var in vect)
//		cout << var << endl;
//	cout << endl;
//	return stream;
//}

void prepareImage(Mat &image) {
	int maxArea = 1024 * 512;
	int area = image.size().area();

	if (area > maxArea) {
		float factor = 2;
		while (area / (factor * factor) > maxArea)
			factor *= 2;
		resize(image, image, Size(), 1/ factor, 1/factor, cv::INTER_CUBIC);
	}
};

int main(int argc, char ** argv) {

	if (argc < 3) {
		cout << "ERROR: not enough arguments" << endl;
		return -1;
	}

	std::vector<path> imageFiles;
	std::vector<path> cascadeFiles;
	std::vector<path> referenceImages;

	imageFiles = getFiles(argv[1], IMAGES);
	cascadeFiles = getFiles(argv[2], CASCADES);
	referenceImages = getFiles(argv[3], IMAGES);

	if (imageFiles.size() == 0) {
		cout << "ERROR: nowhere to detect" << endl;
		return -2;
	}

	if (cascadeFiles.size() == 0 && referenceImages.size() == 0)  {
		cout << "ERROR: nothing to detect" << endl;
		return -3;
	}
	
	vector<Detector*> detectors;

	for each (path file in cascadeFiles) {
		detectors.push_back(new CascadeDetector());
		detectors.back()->setAndLoad(file);
	}

	for each (path image in referenceImages) {
		detectors.push_back(new SurfDetector());
		detectors.back()->setAndLoad(image);
	}


	for each (path imagePath in imageFiles) {

		cout << imagePath.filename() << endl;

		Mat image = imread(imagePath.string(), 1);

		if (!image.empty()) {

			imshow(window_name, image);

			prepareImage(image);

			imshow(window_name, image);

			for each (Detector* detector in detectors) {
				if (detector->isWorking()) {
					cv::waitKey(300);
					detector->detectAndDisplay(image);
					imshow(window_name, image);
				}
			}

			cout << "PRESS ANY KEY or wait a few seconds" << endl;
			cv::waitKey(7000);
		}
		else
			cout << "ERROR: failed to open " << imagePath << endl;
	}

	cout << "DONE" << endl;

	cv::waitKey(0);

	return 0;
}
