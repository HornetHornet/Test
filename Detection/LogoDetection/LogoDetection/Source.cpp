#include "stdafx.h"

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"

String window_name = "LogoDetection";

bool openImage(const path &imagePath, Mat &image) {

	cout << endl << " opening " << imagePath << endl;
	image = imread(imagePath.string());

	if (!image.empty()) {
		cout << " opened " << imagePath.filename().string() << endl;
		return true;
	}
	else {
		cout << "ERROR: failed to open " << imagePath << endl;
		return false;
	}
}

int main(int argc, char ** argv) {

	if (argc < 3) {
		cout << "ERROR: not enough arguments" << endl;
		return -1;
	}

	std::vector<path> scenePaths;
	std::vector<path> objectPaths;

	scenePaths = getFiles(argv[1], IMAGES);
	objectPaths = getFiles(argv[3], IMAGES);

	if (scenePaths.size() == 0) {
		cout << "ERROR: no scenes" << endl;
		return -2;
	}

	if (objectPaths.size() == 0)  {
		cout << "ERROR: no objects" << endl;
		return -3;
	}

	vector<SurfDetector> objects;

	for each (path objectPath in objectPaths) {
		Mat objectImage;

		if (openImage(objectPath, objectImage)) {
			SurfDetector detector("Object " + objectPath.stem().string());
			detector.process(objectImage);
			objects.push_back(detector);
		}
	}

	for each (path scenePath in scenePaths) {

		Mat imgScene;

		SurfDetector sceneSURF("Scene " + scenePath.stem().string());

		if (openImage(scenePath, imgScene)) {

			cout << " processing " << scenePath.filename() << endl;

			sceneSURF.process(imgScene, true);
			if (sceneSURF.isWorking()) {
				for each (SurfDetector object in objects) {
					if (object.isWorking()) {
						object.match(sceneSURF);
					}
				}
			}
		}	
	}

	cout << "DONE" << endl;

	cv::waitKey(0);

	return 0;
}
