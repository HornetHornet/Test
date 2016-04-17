#pragma once

#ifndef GETFILES_H
#define GETFILES_H

#include "stdafx.h"

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

enum fileTypes {IMAGES, VIDEOS, CASCADES};

std::vector<path> getFiles(const path &dir, fileTypes types) {

	string target_name;
	std::vector<string> extentions;
	std::vector<string> imageExtetions;  

	switch (types)
	{
	case IMAGES:
		target_name = "images";
		extentions = { ".jpg", ".png", ".jpeg", ".pb", ".gif" };
		break;
	case VIDEOS:
		target_name = "videos";
		extentions = { ".avi" };
		break;
	case CASCADES:
		target_name = "cascades";
		extentions = { ".xml" };
		break;
	default:
		break;
	}

	std::vector<path> listOfFiles;


	if (!is_directory(dir)) {
		cout << "ERROR: " << dir.string() << " is not a valid path" << endl;
		return listOfFiles;
	}

	for (auto& entry : boost::make_iterator_range(directory_iterator(dir), {})) {

		string fileExtention = entry.path().extension().string();
		to_lower(fileExtention);

		for each (string target in extentions) {
			if (fileExtention == target) {
				listOfFiles.push_back(entry.path());
				break;
			}
		}
	}

	cout << listOfFiles.size() << " " << target_name << " in " << dir << " found" << endl;

	return listOfFiles;
}

#endif