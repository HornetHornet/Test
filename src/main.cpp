#include <iomanip>
#include <queue>
#include <thread>
#include <map>
#include <unordered_map>
#include <future>
#include <gflags/gflags.h>

#include "detection.hpp"
#include "file-utils.hpp"
#include "img-utils.hpp"
#include "geom-utils.hpp"


DEFINE_string(objects, "", "");
DEFINE_string(scenes, "", "");
DEFINE_int32(jobs, 8, "number of cocurrent detection jobs");
DEFINE_int32(obj_min_hess, 400, "");
DEFINE_int32(scn_min_hess, 750, "");
DEFINE_int32(obj_size, 256, "");
DEFINE_int32(scn_size, 780, "");


namespace bfs = boost::filesystem;


static std::string get_session_id()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&tt), "%Y-%m-%d-%H:%M:%S");
	return ss.str();
}


inline double tick(double t)
{
	return ((double) cv::getTickCount() - t) / cv::getTickFrequency();
}


inline bool openImage(const bfs::path &imagePath, cv::Mat &image)
{
	image = cv::imread(imagePath.string(), cv::IMREAD_UNCHANGED);

	if (image.empty())
		return false;

	if (image.channels() == 4)
		makeOpaque(image);

	switch (image.type())
	{
		case (CV_8UC3) :
			break;
		case (CV_16UC3) :
			image.convertTo(image, CV_8UC3, 1.0 / 256);
			break;
		default:
			image.convertTo(image, CV_8UC3);
			break;
	}

	return true;
}


template<typename T, typename R, typename... Args>
R ResultOf(R (T::*)(Args...) const);


template<typename T>
std::vector<T> concurrentlyExecuteScheduledTasks(
		std::queue<std::packaged_task<T()>> &tasks,
		const int n_jobs)
{
	std::vector<std::future<T>> futures;
	std::vector<T> results;

	while (!tasks.empty() or !futures.empty())
	{
		while (!tasks.empty() and futures.size() < n_jobs)
		{
			auto task = std::move(tasks.front());
			tasks.pop();
			futures.push_back(task.get_future());
			std::thread(std::move(task)).detach();
		}

		while (futures.size() == n_jobs or (tasks.empty() and !futures.empty()))
		{
			for (int i = 0; i < futures.size(); ++i)
			{
				switch (futures[i].wait_for(std::chrono::milliseconds(10)))
				{
					case std::future_status::deferred :
						assert(false);
					case std::future_status::timeout :
						continue;
					case std::future_status::ready :
						results.push_back(futures[i].get());
						std::swap(futures[i], futures.back());
						futures.pop_back();
						break;
				}
			}
		}
	}
	return results;
}

void printGFlagsHelp()
{
	std::cerr << "Usage:\n";
	std::vector<google::CommandLineFlagInfo> out;
	google::GetAllFlags(&out);
	for (auto f : out)
	{
		if (f.filename.find("gflags") == std::string::npos)
			std::cerr << google::DescribeOneFlag(f) << std::endl;
	}
}

int main(int argc, char *argv[])
{
	google::ParseCommandLineNonHelpFlags(&argc, &argv, true);
	if (std::string(google::GetArgv()).find("--help") != std::string::npos)
	{
		printGFlagsHelp();
		return 1;
	}

	std::string session_id = get_session_id();
	init_logger(session_id);


	std::vector<bfs::path> obj_paths = list_files(FLAGS_objects, IMAGES);
	std::vector<bfs::path> scn_paths = list_files(FLAGS_scenes, IMAGES);
	const int n_jobs = FLAGS_jobs;

	if (obj_paths.empty() || scn_paths.empty())
	{
		log_err << "objects: " << obj_paths.size() << std::endl;
		log_err << "scenes:  " << scn_paths.size() << std::endl;
		return 1;
	}

	std::vector<std::shared_ptr<KeyPointFeatureDetector>> detectors;
	auto t = (double) cv::getTickCount();

	{
		for (const auto &obj_path : obj_paths)
		{
			std::string object_id =  obj_path.stem().string();
			boost::to_upper<std::string>(object_id);

			log_state << "opening: " << obj_path.string() << std::endl;

			cv::Mat obj_img;
			if (openImage(obj_path, obj_img))
			{
				resizeDown(obj_img, FLAGS_obj_size);
				cv::Mat kp_det_prepared_img =
						KeyPointFeatureDetector::prepare_image(obj_img);
				auto detector = std::make_shared<KeyPointFeatureDetector>(
						object_id, FLAGS_obj_min_hess);

				if (detector->process(kp_det_prepared_img))
					detectors.push_back(detector);
			}
		}
	}

	if (detectors.empty())
	{
		log_state << "ERROR: no working detectors" << std::endl;
		return 2;
	}

	bfs::path results_parent_path = bfs::path("results") / session_id;
	bfs::create_directories(results_parent_path);
	std::map<std::string, std::map<std::string, double>> id_to_stats;
	int scn_cnt = 0;

	for (const auto &scn_path : scn_paths)
	{
		++scn_cnt;
		auto it_start_t = std::chrono::system_clock::now();
		log_state
				<< "scene: " << scn_cnt << " / " << scn_paths.size() << std::endl
				<< "total time: " << tick(t) << " seconds" << std::endl
				<< "avg time: " << tick(t) / scn_cnt << " seconds" << std::endl
				<< std::endl;

		std::string scene_filename = scn_path.filename().string();
		bfs::path result_dst_path = results_parent_path / scene_filename;

		try
		{
			cv::Mat img_scene;
			if (!openImage(scn_path, img_scene))
				continue;

			// prepare and process scene

			preciseResize(img_scene, FLAGS_scn_size);
			cv::Mat kp_det_prepared_img = KeyPointFeatureDetector::prepare_image(img_scene);

			KeyPointFeatureDetector sd_scene("scn_" + scene_filename, FLAGS_scn_min_hess);
			sd_scene.process(kp_det_prepared_img.clone());

			// match with objects

			auto detection_task = [sd_scene](const std::shared_ptr<Detector> obj_det)
			{
				std::vector<std::vector<cv::Point2d>> matches;
				auto kp_det_obj =
						std::dynamic_pointer_cast<KeyPointFeatureDetector>(obj_det);
				if (kp_det_obj)
				{
					matches = kp_det_obj->match(sd_scene);
				}
				return std::make_pair(obj_det->object_id, matches);
			};

			using TaskRet = decltype(ResultOf(&decltype(detection_task)::operator()));
			std::queue<std::packaged_task<TaskRet()>> tasks;
			for (auto &det : detectors)
				tasks.push(std::packaged_task<TaskRet()>(std::bind(detection_task, det)));
			std::vector<TaskRet> results = concurrentlyExecuteScheduledTasks(tasks, n_jobs);

			// show results

			for (const auto &item : results)
			{
				std::string obj_id = item.first;
				std::vector<std::vector<cv::Point2d>> boxes = item.second;

				if (boxes.empty())
					continue;

				cv::Scalar label_clr = makeRandColor();

				for (auto &box : boxes)
				{
					fitPoly(box, img_scene.size());
					drawPoly(img_scene, box, label_clr);
					cv::Point text_pos = geom::centroid(box) - cv::Point2d(30, 0);
					pretty_put_text_line( img_scene, obj_id, text_pos, label_clr);
				}

				cv::imshow("scene", img_scene);
				cv::waitKey(1);

				id_to_stats[obj_id]["encounters"] += boxes.size();
			}

			cv::imwrite(result_dst_path.string(), img_scene);
		}
		catch (const std::exception &e)
		{
			log_err << e.what() << std::endl;
		}
	}

	for (const auto &item : id_to_stats)
	{
		log_state << item.first << std::endl;
		for (const auto &item2 : item.second)
			log_state << "\t" << item2.first << ": " << item2.second << std::endl;
	}

	log_state
			<< std::endl
			<< "DONE" << std::endl
			<< "total time: " << tick(t) << " seconds" << std::endl;

	cv::waitKey();

	return 0;
}

// todo update readme
