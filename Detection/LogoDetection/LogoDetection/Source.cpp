#include "stdafx.h"

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"
#include <thread>

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
		return 1;
	}

	std::vector<path> scn_paths, obj_paths;
	
	cout << endl;

	scn_paths = getFiles(argv[1], IMAGES);
	obj_paths = getFiles(argv[2], IMAGES);

	cout << endl;

	if (scn_paths.size() == 0) {
		cout << "ERROR: no scenes" << endl;
		return 2;
	}

	if (obj_paths.size() == 0)  {
		cout << "ERROR: no objects" << endl;
		return 3;
	}

	string session_id = logg::get_session_id();

	list<SiftDetector> detectors;
	double t = (double)getTickCount();

	{
		std::vector<thread> threads;
		std::vector<Mat> images;

		for each (path obj_path in obj_paths) {
			Mat obj_img;
			if (openImage(obj_path, obj_img)) {
				detectors.push_back(SiftDetector(obj_path.filename().string()));
				trnsf::resizeDown(obj_img, 256);
				images.push_back(obj_img);
			}
		}

		list<SiftDetector>::iterator obj = detectors.begin();
		for (size_t i = 0; i < images.size(); i++) {
			threads.push_back(thread(&SiftDetector::process, &(*obj), images[i]));
			obj++;
		}

		cout << endl << "processed " << 0 << "/ " << threads.size() << " objects";

		for (size_t i = 0; i < threads.size(); i++) {
			threads[i].join();
			cout << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
		}

		cout << endl << endl;
	}

	list<SiftDetector>::iterator it = detectors.begin();
	while (it != detectors.end())	{
		if (!it->isWorking()) {
			logg::tout << " ! can't work with " << it->getName() << endl;
			it = detectors.erase(it);
		}
		else it++;
	}

	if (distance(detectors.begin(), detectors.end()) == 0) {
		logg::tout << "ERROR: no working detectors" << endl;
		return 4;
	}

	create_directory(path("results_" + session_id));

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

		for (list<SiftDetector>::iterator it = detectors.begin(); it != detectors.end(); it++)
			if (it->isWorking())
				threads.push_back(thread(&SiftDetector::match, &(*it), sd_scene, img_scene));

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
