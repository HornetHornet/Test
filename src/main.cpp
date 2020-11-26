#include <thread>

#include "detection.hpp"
#include "file-utils.hpp"
#include "img-utils.hpp"
#include <iomanip>

#define OBJ_MIN_HESS 400
#define SCN_MIN_HESS 750

#define OBJ_SIZE 256
#define SCN_SIZE 780


namespace bfs = boost::filesystem;


static std::string get_session_id() {

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&tt), "%Y-%m-%d-%H:%M:%S");

	return ss.str();
}


inline double tick(double t) {
	return ((double)cv::getTickCount() - t) / cv::getTickFrequency();
}


inline bool openImage(const bfs::path &imagePath, cv::Mat &image) {

	image = cv::imread(imagePath.string(), cv::IMREAD_UNCHANGED);

	if (image.data) {
		log_state << "opened: " << imagePath.filename().string() << std::endl;

		if (image.channels() == 4)
			imgutils::makeOpaque(image);

		switch (image.type()) {
		case(CV_8UC3) : 
			break;
		case(CV_16UC3) : 
			image.convertTo(image, CV_8UC3, 1.0 / 256);
			break;
		default: 
			image.convertTo(image, CV_8UC3);
			break;
		}

		return true;
	}

	log_err << "ERROR: failed to open " << imagePath << std::endl;

	return false;
}


int main(int argc, char ** argv) {
	std::string session_id = get_session_id();
	init_logger(session_id);

	std::string arg_keys =
		"{ o| objects |       | path to images with object}"
		"{ s| data |       | path to images with data}"
		;

	cv::CommandLineParser parser(argc, argv, arg_keys.c_str());

	std::vector<bfs::path> obj_paths = list_files(parser.get<std::string>("o"), IMAGES);
	std::vector<bfs::path> scn_paths = list_files(parser.get<std::string>("s"), IMAGES);

	if (obj_paths.size() == 0 || scn_paths.size() == 0) {
		log_err << obj_paths.size() << " objects and " << scn_paths.size() << " data" << std::endl;
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
//		log_state << std::endl << "processed " << 0 << "/ " << threads.size() << " objects";
//
//		for (size_t i = 0; i < threads.size(); i++) {
//			threads[i].join();
//			log_state << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
//		}
//
//		log_state << std::endl;
//	}

	{
		for(const auto & obj_path : obj_paths) {
			cv::Mat obj_img;
			if (openImage(obj_path, obj_img)) {
				detectors.push_back(SiftDetector(obj_path.filename().string(), OBJ_MIN_HESS));
				imgutils::resizeDown(obj_img, OBJ_SIZE);
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
//		log_state << std::endl << "processed " << 0 << "/ " << threads.size() << " objects";
//
//		for (size_t i = 0; i < threads.size(); i++) {
//			threads[i].join();
//			log_state << '\r' << "processed " << i + 1 << "/ " << threads.size() << " objects";
//		}

	}

	auto it = detectors.begin();
	while (it != detectors.end())	{
		if (it->isWorking()) 
			it++;
		else {
			log_err << "Failed to find at least " << MIN_POINTS << " points on "
				<< it->getName() << ", won't search for it"<< std::endl;
			it = detectors.erase(it);
		}
	}

	if (distance(detectors.begin(), detectors.end()) == 0) {
		log_state << "ERROR: no working detectors" << std::endl;
		return 2;
	}

	int scn_proc = 0;

	bfs::create_directories(bfs::path("results/" + session_id));

	for (const auto & scn_path : scn_paths) {

		log_state << std::endl << std::endl << "time passed: " << tick(t) << " seconds"
			<< std::endl << "scene: " << ++scn_proc << "/ " << scn_paths.size() << " " << std::endl;

		try {
			cv::Mat img_scene;

			if (!openImage(scn_path, img_scene))
				continue;

			imgutils::preciseResize(img_scene, SCN_SIZE);

			SiftDetector sd_scene("scn_" + scn_path.filename().string(), SCN_MIN_HESS);
			sd_scene.process(img_scene.clone());

			if (!sd_scene.isWorking())
				continue;

			for (auto & det : detectors){
//			log_state << std::endl;
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
			log_err << e.what() << std::endl;
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
//		log_state << std::endl;
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

	log_state << std::endl << "DONE" << std::endl
		<< "total time: " << tick(t) << " seconds" << std::endl;
//		<< "detections: " << SiftDetector::detections << std::endl;

	cv::waitKey();

	return 0;
}

// todo calc stats: avg, median time, how many objects of each type detected
// todo namespaces -> utils
// todo better exception handling
// todo use gflags?
// todo better debug demonstration? move it to main?
// todo better comments
// todo update readme
