#include <iostream>
#include <fstream>
#include <direct.h>
#include <ctime>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "GetFiles.h"

bool checkDir(const path &dir) {
	if (!is_directory(dir)) {
		cout << "ERROR:" << dir << " is invalid" << endl;
		return false;
	}
	return true;
}

string readWhole(string file) {
	string text;
	std::ifstream fin(file);

	if (!fin.is_open()) {
		cout << "ERROR: cannot open " << file << endl;
		return text;
	}

	text = string ((
		std::istreambuf_iterator<char>(fin)),
		std::istreambuf_iterator<char>());

	cout << text << endl;

	return text;
}

int main(int argc, char** argv) {

	if (argc < 7) {
		cout << "ERROR: not enough arguments" << endl;
		return -1;
	}

	path opencvCreateSamples{ argv[1]};
	path opencvTraincascadePath{ argv[2] };

	path refImageFolder{ argv[3] };
	path scriptFolder { argv[4]};
	path vecFolder { argv[5] };
	path cascadeFolder { argv[6] };

	if (!is_directory(vecFolder)) {
		_mkdir(vecFolder.string().c_str());
	}

	if (!is_directory(cascadeFolder)) {
		_mkdir(cascadeFolder.string().c_str());
	}

	if ( !exists(opencvCreateSamples)
		|| !exists(opencvTraincascadePath)
		|| (!checkDir(refImageFolder) && !exists(refImageFolder) )
		|| !checkDir(scriptFolder)
		|| !checkDir(vecFolder)
		|| !checkDir(cascadeFolder)) {
		cout << "ERROR: invalid paths" << endl;
		return -1;
	}

	cout << endl << "opencv opencv_createsamples.exe: " << opencvCreateSamples << endl;
	cout << "opencv opencv_traincascade.exe: " << opencvTraincascadePath << endl;
	cout << "scripts folder: " << scriptFolder << endl;
	cout << "vectors folder: " << vecFolder << endl;
	cout << "cascades folder: " << cascadeFolder << endl;
	cout << "reference image folder: " << refImageFolder << endl << endl;

	vector<path> refImages = getFiles(refImageFolder, IMAGES);

	string cs_params = readWhole("params_for_createsamples.txt");
	string tc_params = readWhole("params_for_traincascade.txt");

	if (cs_params.empty() || tc_params.empty()) {
		cout << "ERROR: no paramets given" << endl;
	}

	std::ofstream cs_script_out(scriptFolder.string() + "\\" + "run_createsamples_generated.bat");
	std::ofstream tc_script_out(scriptFolder.string() + "\\" + "run_traincascade_generated.bat");
	
	if (!cs_script_out.is_open() || !tc_script_out.is_open()) {
		cout << "ERROR: cannot create scripts" << endl;
		return -1;
	}

	for each (path refImage in refImages) {
		string name = refImage.stem().string();
		cout << "path: " << refImage << endl;
		cout << "name: " << name << endl << endl;

		string vectorPath = vecFolder.string() + "\\" + name + ".vec" ;

		cs_script_out << opencvCreateSamples << "^" << endl;
		cs_script_out << " -vec \"" << vectorPath << "\"^" << endl;
		cs_script_out << " -img " << refImage << "^" << endl;
		cs_script_out << cs_params << endl;

		string cascadePath = cascadeFolder.string() + "\\" + name;
		_mkdir(cascadePath.c_str());

		tc_script_out << opencvTraincascadePath << "^" << endl;
		tc_script_out << " -data \"" << cascadePath << "\"^" << endl;
		tc_script_out << " -vec \"" << vectorPath << "\"^" << endl;
		tc_script_out << tc_params << endl;
		
		if (refImage == refImages.front()) {
			cs_script_out << "REM you may QUIT now or CONTINUE" << endl;
			tc_script_out << "REM you may QUIT now or CONTINUE" << endl;
			cs_script_out << endl << "pause" << endl;
			tc_script_out << endl << "pause" << endl;
		}

		cs_script_out << endl << endl;
		tc_script_out << endl << endl;
	}

	cs_script_out << "pause" << endl;
	tc_script_out << "pause" << endl;

	return 0;
}