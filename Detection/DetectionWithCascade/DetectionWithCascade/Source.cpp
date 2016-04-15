#include "stdafx.h"

#include <opencv2/highgui.hpp>

#include "CascadeWrapper.h"
#include <boost/algorithm/string.hpp>    

using namespace boost::filesystem;
using namespace boost::algorithm;

String window_name = "Detection";

template <class T> ostream &operator << (ostream &stream, const std::vector <T> &vect) {
	for each (T var in vect)
		cout << var << endl;
	cout << endl;
	return stream;
}

std::vector<path> getFiles(const string &strDir, const vector<string> &extentions, string target_name = "objects") {

	std::vector<path> listOfFiles;

	path phDir{ strDir };

	if (!is_directory(phDir)) {
		cout << "ERROR: " << phDir.string() << " is not a valid path" << endl;
		return listOfFiles;
	}

	for (auto& entry : boost::make_iterator_range(directory_iterator(phDir), {})) {

		string fileExtention = entry.path().extension().string();
		to_lower(fileExtention);

		for each (string target in extentions) {
			if (fileExtention == target) {
				listOfFiles.push_back(entry.path());
				break;
			}
		}
	}

	cout << listOfFiles.size() << " " << target_name << " in " << strDir << " found" << endl;

	return listOfFiles;
}

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

	std::vector<path> cascadeFiles;
	std::vector<path> imageFiles;

	{
		const std::vector<string> cascadeExtentions({ ".xml" });
		const std::vector<string> imageExtetions({ ".jpg", ".png", ".jpeg", ".pb", ".gif" });

		cascadeFiles = getFiles(argv[1], cascadeExtentions, "cascades");
		imageFiles = getFiles(argv[2], imageExtetions, "images");
	}

	if (cascadeFiles.size() == 0 && imageFiles.size() == 0)
		return -1;


	vector<CascadeWrapper> cascades(cascadeFiles.size());

	for (size_t i = 0; i < cascades.size(); i++) {
		cascades[i].setAndLoad(cascadeFiles[i]);
	}

	for each (path imagePath in imageFiles) {

		cout << imagePath.filename() << endl;

		Mat image = imread(imagePath.string(), 1);

		if (!image.empty()) {

			imshow(window_name, image);

			prepareImage(image);

			imshow(window_name, image);

			for each (CascadeWrapper cascade in cascades) {
				cv::waitKey(100);
				cascade.detectAndDisplay(image);
				imshow(window_name, image);
				cv::waitKey(10000);
			}
		}
		else
			cout << "ERROR: failed to open " << imagePath << endl;
	}

	cout << "DONE" << endl;

	waitKey(0);

	return 0;
}
