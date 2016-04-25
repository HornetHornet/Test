#include "stdafx.h"

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"
#include <thread>

String window_name = "LogoDetection";

inline double tick(double t) {
	double passed = ((double)getTickCount() - t) / getTickFrequency();	
	return passed;
}

inline bool openImage(const path &imagePath, Mat &image) {
	image = imread(imagePath.string(), CV_LOAD_IMAGE_UNCHANGED);
	if (image.data) {
		cout << "opened: " << imagePath.filename().string() << endl;
		return true;
	}
	logg::tout << "ERROR: failed to open " << imagePath << endl;
	return false;
}

int main(int argc, char ** argv) {
	
	if (argc < 3) {
		cout << "ERROR: not enough arguments" << endl;
		return -1;
	}

	std::vector<path> scn_paths, obj_paths;
	
	cout << endl;

	scn_paths = getFiles(argv[1], IMAGES);
	obj_paths = getFiles(argv[2], IMAGES);

	cout << endl;

	if (scn_paths.size() == 0) {
		cout << "ERROR: no scenes" << endl;
		return -2;
	}

	if (obj_paths.size() == 0)  {
		cout << "ERROR: no objects" << endl;
		return -3;
	}

	string session_id = logg::get_session_id();

	create_directory(path("results_" + session_id));

	//list<SiftDetector> objects;
	std::vector<SiftDetector> objects;
	double t = (double)getTickCount();

	{
		vector<thread> threads;
		vector<Mat> images;

		for each (path obj_path in obj_paths) {
			Mat obj_img;
			if (openImage(obj_path, obj_img)) {
				objects.push_back(SiftDetector(obj_path.filename().string()));
				trnsf::resizeDown(obj_img, 256);
				images.push_back(obj_img);
			}
		}

		//for (size_t i = 0; i < images.size(); i++) {
		//	list<SiftDetector>::iterator obj = objects.begin();
		//	threads.push_back(thread(&SiftDetector::process, obj, images[i]));
		//	obj++;
		//}

		for (size_t i = 0; i < images.size(); i++)
			threads.push_back(thread(&SiftDetector::process, &objects[i], images[i]));

		cout << endl << "processed " << 0 << "/ " << threads.size() << " objects";

		for (size_t i = 0; i < threads.size(); i++) {
			threads[i].join();
			cout << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
		}

		cout << endl << endl;
	}

	for each (SiftDetector detector in objects) {
		if (!detector.isWorking())
			logg::tout << " ! can't work with " << detector.getName() << endl;
	}

	//for (list<SiftDetector>::iterator it = objects.begin(); it != objects.end(); it++) {
	//	if (!it->isWorking()) {
	//		tout << "\can't work with " << detector.getName() << endl;
	//		objects.erase(it);
	//	}
	//}

	int scn_proc = 0;

	for each (path scn_path in scn_paths) {

		logg::tout << endl << endl << "time passed: " << tick(t) << " seconds"
			<< endl << "scene: " << ++scn_proc << "/ " << scn_paths.size() << " " << endl;
			

		Mat img_scene;

		if (!openImage(scn_path, img_scene))
			continue;

		trnsf::preciseResize(img_scene, 780);

		SiftDetector sd_scene("scn_" + scn_path.filename().string());
		sd_scene.process(img_scene.clone());

		if (!sd_scene.isWorking())
			continue;

		int obj_proc = 0;
		vector<thread> threads;

		cout << endl;

		for (size_t i = 0; i < objects.size(); i++) {
			if (objects[i].isWorking())
				threads.push_back(thread(&SiftDetector::match, &objects[i], sd_scene, img_scene));
		}

		for (size_t i = 0; i < threads.size(); i++)
			threads[i].join();

		imwrite("results_" + session_id + "/res_for_" + scn_path.stem().string() + ".jpg", img_scene);
	}

	logg::tout << endl << "DONE" << endl
		<< "total time: " << tick(t) << " seconds" << endl
		<< "detections:  " << SiftDetector::detections << endl;

	cv::waitKey();

	return 0;
}
