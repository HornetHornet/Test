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

inline double tick(int64 t, bool reload = false) {
	double passed = ((double)getTickCount() - t) / getTickFrequency();	
	t = reload ? getTickCount() : t;
	return passed;
}

string get_session_id() {

	using namespace std;
	using namespace std::chrono;

	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);

	struct tm local_tm;
	localtime_s(&local_tm, &tt);

	std::stringstream session_id;
	session_id << "D" << local_tm.tm_mday
		<< "_H" << local_tm.tm_hour
		<< "_M" << (local_tm.tm_min / 10) * 10;

	return session_id.str();
}

bool openImage(const path &imagePath, Mat &image) {

	//tout << " opening " << imagePath << endl;
	image = imread(imagePath.string(), CV_LOAD_IMAGE_UNCHANGED);

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

	string session_id = get_session_id();
	fout.open("logo_" + session_id + ".log", std::ofstream::app);
	Tee tee(std::cout, fout);
	tout.open(tee);
	create_directory(path("results_" + session_id));

	std::vector<SiftDetector> objects;
	double t = (double)getTickCount();
	
	for each (path objectPath in path_objects) {
		Mat objectImage;
		cout << " object " << objectPath.stem() << endl;

		if (openImage(objectPath, objectImage)) {
			SiftDetector detector(objectPath.filename().string());
			detector.process(objectImage);

			if (detector.isWorking())
				objects.push_back(detector);
			else
				tout << "ERROR: can't work with " << objectPath.filename() << endl;
		}
	}

	tout << "preproc time: " << tick(t, true) << endl;

	int detections = 0;
	int scn_proc = 0;

	for each (path path_scene in path_scenes) {

		tout << "time passed: " << tick(t) << " seconds" << endl
			<< endl << "scene " << ++scn_proc << "/ " << path_scenes.size() << " " << path_scene.stem() << endl;

		Mat img_scene;
		SiftDetector sd_scene("scn_" + path_scene.filename().string());

		if (openImage(path_scene, img_scene)) {

			trnsf::resize(img_scene, 780);
			sd_scene.process(img_scene.clone(), true);

			if (sd_scene.isWorking()) {
				int obj_proc = 0;
				for each (SiftDetector object in objects) {
					//cv::waitKey(50);

					cout << endl << "object " << ++obj_proc << "/ " << objects.size() << endl;

					if (object.match(sd_scene, img_scene)) {
						tout << " got " << object.getName() << " on " << sd_scene.getName() << endl;
						detections++;
						//break;
					}
				}
				//imshow("Logo Detection", img_scene);
				imwrite("results_" + session_id + "/res_for_" + path_scene.stem().string() + ".jpg", img_scene);
			}
		}
	}

	tout << "DONE" << endl
		<< "total time: " << tick(t) << " seconds" << endl
		<< "detections:  " << detections << endl;
	cv::waitKey(0);

	return 0;
}
