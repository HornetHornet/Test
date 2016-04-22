#include "stdafx.h"

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;

static std::ofstream fout;
static TeeStream tout;

String window_name = "LogoDetection";

string getSessionTime() {

	using namespace std;
	using namespace std::chrono;

	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);

	struct tm local_tm;
	localtime_s(&local_tm, &tt);

	std::stringstream session;
	session << "D" << local_tm.tm_mday
		<< "_H" << local_tm.tm_hour
		<< "_M" << (local_tm.tm_min / 10) * 10;

	return session.str();
}

bool openImage(const path &imagePath, Mat &image) {

	//tout << " opening " << imagePath << endl;
	image = imread(imagePath.string());

	if (!image.empty()) {
		tout << " opened " << imagePath.filename().string() << endl << endl;
		return true;
	}
	
	tout << "ERROR: failed to open " << imagePath << endl << endl;
	return false;
}

int main(int argc, char ** argv) {
	
	if (argc < 3) {
		cout << "ERROR: not enough arguments" << endl;
		return -1;
	}

	std::vector<path> path_scenes;
	std::vector<path> path_objects;

	path_scenes = getFiles(argv[1], IMAGES);
	path_objects = getFiles(argv[2], IMAGES);

	if (path_scenes.size() == 0) {
		cout << "ERROR: no scenes" << endl;
		return -2;
	}

	if (path_objects.size() == 0)  {
		cout << "ERROR: no objects" << endl;
		return -3;
	}

	string sessionTime = getSessionTime();
	fout.open("logo_" + sessionTime + ".log", std::ofstream::app);

	Tee tee(std::cout, fout);
	tout.open(tee);

	create_directory(path("results_" + sessionTime));

	std::vector<SurfDetector> objects;
	double t = (double)getTickCount();
	
	for each (path objectPath in path_objects) {
		Mat objectImage;
		cout << " object " << objectPath.stem() << endl;

		if (openImage(objectPath, objectImage)) {
			SurfDetector detector("obj_" + objectPath.filename().string());
			detector.process(objectImage);

			if (detector.isWorking())
				objects.push_back(detector);
			else
				tout << "ERROR: can't work with " << objectPath.filename() << endl;
		}
	}

	tout << "preproc time: " << ((double)getTickCount() - t) / getTickFrequency() << endl;
	t = getTickCount();

	int detections = 0;
	int scene_processed = 0;

	for each (path path_scene in path_scenes) {

		cout << endl << "scene " << path_scene.stem() << endl;

		Mat img_scene;
		SurfDetector surf_scene("scn_" + path_scene.filename().string());

		if (openImage(path_scene, img_scene)) {

			shrinkTo(img_scene, 780);
			surf_scene.process(img_scene.clone(), true);

			if (surf_scene.isWorking()) {
				int obj_processed = 0;
				for each (SurfDetector object in objects) {
					if (object.match(surf_scene, img_scene)) {
						tout << " got " << object.getName() << " on " << surf_scene.getName() << endl;
						detections++;
						break;
					}
					cout << endl << "object " << ++obj_processed << "/ " << objects.size() << endl;
					cv::waitKey(50);
				}
				imshow("Logo Detection", img_scene);
				imwrite("results_" + sessionTime + "/res_for_" + path_scene.stem().string() + ".jpg", img_scene);
			}
		}
		cout << " scenes " << ++scene_processed << "/ " << path_scenes.size() << endl;
	}

	tout << "DONE" << endl;
	tout << "total time: " << ((double)getTickCount() - t) / getTickFrequency() << " seconds" << endl;
	tout << "detections:  " << detections << endl;
	cv::waitKey(0);

	return 0;
}
