#include "stdafx.h"

#include <thread>

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"

#define OBJ_MIN_HESS 400
#define SCN_MIN_HESS 750

#define OBJ_SIZE 256
#define SCN_SIZE 780

inline double tick(double t) {
	return ((double)getTickCount() - t) / getTickFrequency();
}

inline bool openImage(const path &imagePath, Mat &image) {

	image = imread(imagePath.string(), CV_LOAD_IMAGE_UNCHANGED);

	if (image.data) {
		cout << "opened: " << imagePath.filename().string() << endl;

		if (image.channels() == 4)
			trnsf::makeOpaque(image);

		switch (image.type()) {
		case(CV_8UC3) : 
			break;
		case(CV_16UC3) : 
			image.convertTo(image, CV_8UC3, 1.0 / 256); break;
		default: 
			image.convertTo(image, CV_8UC3); break;
		}

		return true;
	}

	logg::tout << "ERROR: failed to open " << imagePath << endl;

	return false;
}


int main(int argc, char ** argv) {
	
	char* keys = 
		"{ o| objects_path |       | path to images with object}"
		"{ s| scenes_path  |       | path to images with scenes}"
		"{ r| recursive    | false | whether to search for images recursivly}";

	CommandLineParser parser(argc, argv, keys);
	parser.printParams();

	cout << endl;
	std::vector<path> obj_paths = getFiles(parser.get<string>("o"), IMAGES, parser.get<bool>("r"));
	std::vector<path> scn_paths = getFiles(parser.get<string>("s"), IMAGES, parser.get<bool>("r"));
	cout << endl;

	if (obj_paths.size() == 0 || scn_paths.size() == 0) {
		cout << "ERROR: found " << obj_paths.size() << " objects and " << scn_paths.size() << " scenes" << endl;
		return 1;
	}

	list<SiftDetector> detectors;
	double t = (double)getTickCount();

	{
		std::vector<thread> threads;
		std::vector<Mat> images;

		for each (path obj_path in obj_paths) {
			Mat obj_img;
			if (openImage(obj_path, obj_img)) {
				detectors.push_back(SiftDetector(obj_path.filename().string(), OBJ_MIN_HESS));
				trnsf::resizeDown(obj_img, OBJ_SIZE);
				images.push_back(obj_img);
			}
		}

		auto it = detectors.begin();
		for (size_t i = 0; i < images.size(); i++) {
			threads.push_back(thread(&SiftDetector::process, &(*it), images[i]));
			it++;
		}

		cout << endl << "processed " << 0 << "/ " << threads.size() << " objects";

		for (size_t i = 0; i < threads.size(); i++) {
			threads[i].join();
			cout << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
		}

		cout << endl;
	}

	auto it = detectors.begin();
	while (it != detectors.end())	{
		if (it->isWorking()) 
			it++;
		else {
			logg::tout << " ! failed to find at least " << MIN_POINTS << " points on " 
				<< it->getName() << ", won't search for it"<< endl;
			it = detectors.erase(it);
		}
	}

	if (distance(detectors.begin(), detectors.end()) == 0) {
		logg::tout << "ERROR: no working detectors" << endl;
		return 2;
	}

	int scn_proc = 0;
	string session_id = logg::get_session_id();

	create_directory(path("results_" + session_id));

	for each (path scn_path in scn_paths) {

		logg::tout << endl << endl << "time passed: " << tick(t) << " seconds"
			<< endl << "scene: " << ++scn_proc << "/ " << scn_paths.size() << " " << endl;
			
		Mat img_scene;

		if (!openImage(scn_path, img_scene))
			continue;

		trnsf::preciseResize(img_scene, SCN_SIZE);

		SiftDetector sd_scene("scn_" + scn_path.filename().string(), SCN_MIN_HESS);
		sd_scene.process(img_scene.clone());

		if (!sd_scene.isWorking())
			continue;


		cout << endl;

		vector<thread> threads;

		for (auto it = detectors.begin(); it != detectors.end(); it++) {
			if (it->isWorking())
				threads.push_back(thread(&SiftDetector::match, &(*it), sd_scene, img_scene));
		}

		for (size_t i = 0; i < threads.size(); i++)
			threads[i].join();

		imwrite("results_" + session_id + "/res_for_" + scn_path.stem().string() + ".jpg", img_scene);
	}

	logg::tout << endl << "DONE" << endl
		<< "total time: " << tick(t) << " seconds" << endl
		<< "detections: " << SiftDetector::detections << endl;

	cv::waitKey();

	return 0;
}
