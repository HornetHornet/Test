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

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

	if (argc < 7) {
		cout << "ERROR: not enought argumets" << endl;
		return -1;
	}

	// apply given parametrs
	string videoPath = argv[1];
	string destinationPath = argv[2];
	unsigned int NumberOfExamplesRequired = static_cast<unsigned int> (stoi(argv[3]));
	unsigned int period = static_cast<unsigned int> (max(1, stoi(argv[4])));
	unsigned int requiredWidth = static_cast<unsigned int> (stoi(argv[5]));
	unsigned int requiredHeight = static_cast<unsigned int> (stoi(argv[6]));

	// set names, directiories, etc
	string vidName = videoPath.substr(videoPath.find('/') + 1, videoPath.find('.') - videoPath.find('/') - 1);
	string imageDir = "img";
	string descriptionFile = "bg.txt";
	ofstream fout;
	time_t t = time(0); // get system time to make image names unique for every session

	fout.open(destinationPath + "//" + descriptionFile, ostream::app);

	if (!fout.is_open()) {  // create folders if needed
		cout << "failed to open a folder, creating..." << endl;

		_mkdir(destinationPath.c_str());
		_mkdir((destinationPath + "//" + imageDir).c_str());

		fout.open(destinationPath + "//" + descriptionFile, ostream::app);

		if (!fout.is_open()) {
			cout << "ERROR: failed to open and create a folder" << destinationPath << endl;
			return -1;
		}
	}

	VideoCapture cap(videoPath);

	if (!cap.isOpened()) {
		cout << "ERROR: failed to open a video" << endl;
		return -1;
	}

	Mat frame;

	for (int i = 0; i < NumberOfExamplesRequired; i++) {

		for (int j = 0; j < period && cap.grab(); j++);  // skip frames	

		cap >> frame;

		if (frame.empty())
			break;

		resize(frame, frame, Size(requiredWidth, requiredHeight), 0, 0, cv::INTER_LANCZOS4);

		string imageName(vidName + "_" + to_string(t) + "_" + to_string(i) + ".jpg");
		fout << imageDir + "//" + imageName << '\n';  // create a record in the description file
		imwrite(destinationPath + "//" + imageDir + "//" + imageName, frame, { 50 });

		cout << "writing " << destinationPath + "/" + imageDir + "/" + imageName << endl;
	}

	cout << "DONE" << endl;

	return 0;
}