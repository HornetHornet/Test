#include <thread>

#include "Detectors.h"
#include "GetFiles.h"
#include "GeneralTransforms.h"

#define OBJ_MIN_HESS 400
#define SCN_MIN_HESS 750

#define OBJ_SIZE 256
#define SCN_SIZE 780

inline double tick(double t) {
	return ((double)cv::getTickCount() - t) / cv::getTickFrequency();
}

inline bool openImage(const path &imagePath, cv::Mat &image) {

	image = cv::imread(imagePath.string(), cv::IMREAD_UNCHANGED);

	if (image.data) {
		std::cout << "opened: " << imagePath.filename().string() << std::endl;

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

	logg::tout << "ERROR: failed to open " << imagePath << std::endl;

	return false;
}


int main(int argc, char ** argv) {
	
	char* keys = 
		"{ o| objects |       | path to images with object}"
		"{ s| data |       | path to images with data}"
		"{ r| recursive    | false | whether to search for images recursivly}";

	cv::CommandLineParser parser(argc, argv, keys);
//	parser.;

	std::cout << std::endl;
	std::vector<path> obj_paths = getFiles(parser.get<std::string>("o"), IMAGES, parser.get<bool>("r"));
	std::vector<path> scn_paths = getFiles(parser.get<std::string>("s"), IMAGES, parser.get<bool>("r"));
	std::cout << std::endl;

	if (obj_paths.size() == 0 || scn_paths.size() == 0) {
		std::cout << "ERROR: found " << obj_paths.size() << " objects and " << scn_paths.size() << " data" << std::endl;
		return 1;
	}

	std::vector<SiftDetector> detectors;
	double t = (double)cv::getTickCount();

//	{
//		std::vector<std::thread> threads;
//		std::vector<cv::Mat> images;
//
//		for(const auto & obj_path : obj_paths) {
//			cv::Mat obj_img;
//			if (openImage(obj_path, obj_img)) {
//				detectors.push_back(SiftDetector(obj_path.filename().string(), OBJ_MIN_HESS));
//				trnsf::resizeDown(obj_img, OBJ_SIZE);
//				images.push_back(obj_img);
//			}
//		}
//
//		auto it = detectors.begin();
//		for (size_t i = 0; i < images.size(); i++) {
//			threads.push_back(std::thread(&SiftDetector::process, &(*it), images[i]));
//			it++;
//		}
//
//		std::cout << std::endl << "processed " << 0 << "/ " << threads.size() << " objects";
//
//		for (size_t i = 0; i < threads.size(); i++) {
//			threads[i].join();
//			std::cout << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
//		}
//
//		std::cout << std::endl;
//	}

	{
		for(const auto & obj_path : obj_paths) {
			cv::Mat obj_img;
			if (openImage(obj_path, obj_img)) {
				detectors.push_back(SiftDetector(obj_path.filename().string(), OBJ_MIN_HESS));
				trnsf::resizeDown(obj_img, OBJ_SIZE);
//				images.push_back(obj_img);
//				detectors.push_back(SiftDetector());
				detectors.back().process(obj_img);
			}
		}

//		auto it = detectors.begin();
//		for (size_t i = 0; i < images.size(); i++) {
//			threads.push_back(std::thread(&SiftDetector::process, &(*it), images[i]));
//			it++;
//		}
//
//		std::cout << std::endl << "processed " << 0 << "/ " << threads.size() << " objects";
//
//		for (size_t i = 0; i < threads.size(); i++) {
//			threads[i].join();
//			std::cout << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
//		}

		std::cout << std::endl;
	}

	auto it = detectors.begin();
	while (it != detectors.end())	{
		if (it->isWorking()) 
			it++;
		else {
			logg::tout << "Failed to find at least " << MIN_POINTS << " points on "
				<< it->getName() << ", won't search for it"<< std::endl;
			it = detectors.erase(it);
		}
	}

	if (distance(detectors.begin(), detectors.end()) == 0) {
		logg::tout << "ERROR: no working detectors" << std::endl;
		return 2;
	}

	int scn_proc = 0;
	std::string session_id = logg::get_session_id();

	create_directory(path("results/" + session_id));

	for (const auto & scn_path : scn_paths) {

		logg::tout << std::endl << std::endl << "time passed: " << tick(t) << " seconds"
			<< std::endl << "scene: " << ++scn_proc << "/ " << scn_paths.size() << " " << std::endl;

		try {
			cv::Mat img_scene;

			if (!openImage(scn_path, img_scene))
				continue;

			trnsf::preciseResize(img_scene, SCN_SIZE);

			SiftDetector sd_scene("scn_" + scn_path.filename().string(), SCN_MIN_HESS);
			sd_scene.process(img_scene.clone());

			if (!sd_scene.isWorking())
				continue;

			for (auto & det : detectors){
//			std::cout << std::endl;
				det.match(sd_scene, img_scene);
			}


//		std::vector<std::thread> threads;
//
//		for (int i = 0; i < detectors.size(); ++i){
//			if (it->isWorking())
//				threads.push_back(std::thread(&SiftDetector::match, detectors[i], sd_scene, img_scene));
//		}
////		for (auto it = detectors.begin(); it != detectors.end(); it++) {
////			if (it->isWorking())
////				threads.push_back(std::thread(&SiftDetector::match, &(*it), sd_scene, img_scene));
////		}
//
//		for (size_t i = 0; i < threads.size(); i++)
//			threads[i].join();

			cv::imwrite("results_" + session_id + "/res_for_" + scn_path.stem().string() + ".jpg", img_scene);

		} catch (const std::exception & e){
			std::cerr << e.what() << std::endl;
		}

	}


//	for (const auto & scn_path : scn_paths) {
//
//		logg::tout << std::endl << std::endl << "time passed: " << tick(t) << " seconds"
//			<< std::endl << "scene: " << ++scn_proc << "/ " << scn_paths.size() << " " << std::endl;
//
//		cv::Mat img_scene;
//
//		if (!openImage(scn_path, img_scene))
//			continue;
//
//		trnsf::preciseResize(img_scene, SCN_SIZE);
//
//		SiftDetector sd_scene("scn_" + scn_path.filename().string(), SCN_MIN_HESS);
//		sd_scene.process(img_scene);
//
//		if (!sd_scene.isWorking())
//			continue;
//
//
//		std::cout << std::endl;
//
//		std::vector<std::thread> threads;
//
//		for (int i = 0; i < detectors.size(); ++i){
//			if (it->isWorking())
//				threads.push_back(std::thread(&SiftDetector::match, detectors[i], sd_scene, img_scene));
//		}
////		for (auto it = detectors.begin(); it != detectors.end(); it++) {
////			if (it->isWorking())
////				threads.push_back(std::thread(&SiftDetector::match, &(*it), sd_scene, img_scene));
////		}
//
//		for (size_t i = 0; i < threads.size(); i++)
//			threads[i].join();
//
//		imwrite("results_" + session_id + "/res_for_" + scn_path.stem().string() + ".jpg", img_scene);
//	}

	logg::tout << std::endl << "DONE" << std::endl
		<< "total time: " << tick(t) << " seconds" << std::endl;
//		<< "detections: " << SiftDetector::detections << std::endl;

	cv::waitKey();

	return 0;
}

// todo cleanup
// todo calc stats: avg, median time, how many objects of each type detected
// todo namespaces -> utils
// todo better exception handling
// todo refactor logging
// todo use gflags?
// todo better debug demonstration? move it to main?
// todo better comments
// todo update readme
