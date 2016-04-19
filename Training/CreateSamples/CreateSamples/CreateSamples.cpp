/*
Application creates sample images from a given video and a decription file
for the cascade classificator training
*/

#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <direct.h>
#include <ctime>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "opencv2/imgproc.hpp"

#include "GetFiles.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

	if (argc < 8) {
		cout << "ERROR: not enought argumets" << endl;
		return -1;
	}

	// apply given parametrs
	path videoPath{ argv[1] };
	path destinationPath{ argv[2] };
	path absPath{ argv[3] };

	unsigned int NumberOfExamplesRequired = static_cast<unsigned int> (stoi(argv[4]));
	unsigned int period = static_cast<unsigned int> (max(1, stoi(argv[5])));
	unsigned int requiredWidth = static_cast<unsigned int> (stoi(argv[6]));
	unsigned int requiredHeight = static_cast<unsigned int> (stoi(argv[7]));

	// set names, directories, etc
	string imageDir = "img";
	
	string cs_decsription = "cs_bg.txt";
	string tc_decsription = "tc_bg.txt";

	std::ofstream cs_fout;
	std::ofstream tc_fout;

	cs_fout.open(destinationPath.string() + "//" + cs_decsription, ostream::app);
	tc_fout.open(destinationPath.string() + "//" + tc_decsription, ostream::app);

	if (!cs_fout.is_open() || !tc_fout.is_open()) {  // create folders if needed
		cout << "failed to open a folder, creating..." << endl;

		_mkdir(destinationPath.string().c_str());
		_mkdir((destinationPath.string() + "//" + imageDir).c_str());

		cs_fout.open(destinationPath.string() + "//" + cs_decsription, ostream::app);
		tc_fout.open(destinationPath.string() + "//" + tc_decsription, ostream::app);

		if (!cs_fout.is_open() || !tc_fout.is_open()) {
			cout << "ERROR: failed to open and create a folder" << destinationPath << endl;
			return -1;
		}
	}

	vector<path> videos = getFiles(videoPath, VIDEOS);

	for each (path video in videos) {

		cout << video << endl;

		VideoCapture cap(video.string());
		Mat frame;

		time_t theTime = time(NULL); // get system time to make image names unique for every session

		if (!cap.isOpened()) {
			cout << "ERROR: failed to open " << video << endl;
			continue;
		} 
		else
			cout << "opened " << video << endl;
		
		for (int i = 0; i < NumberOfExamplesRequired; i++) {
			for (int j = 0; j < period && cap.grab(); j++);  // skip frames	

			cap >> frame;

			if (frame.empty()) break;

			//resize(frame, frame, Size(requiredWidth, requiredHeight), 0, 0, cv::INTER_LANCZOS4);

			string imageName(videoPath.stem().string() + "_" + to_string(theTime) + "_" + to_string(i) + ".jpg");
			string bgTxtRecord = imageDir + "\\" + imageName;

			// save image
			imwrite(destinationPath.string() + "\\" + bgTxtRecord, frame, { 50 });

			// create a record in the description file for opencv_createsamples
			cs_fout << bgTxtRecord << endl; 

			// create a record in the description file for opencv_traincascade
			tc_fout << absPath.string() + "\\" + bgTxtRecord << endl; 

			cout << "writing " << bgTxtRecord << endl;
		}
	}

	cout << "DONE" << endl;

	return 0;
}