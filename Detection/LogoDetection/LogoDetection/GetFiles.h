#pragma once

#ifndef GETFILES_H
#define GETFILES_H

#include <iostream>
#include <stdio.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 

using namespace std;
using namespace boost::filesystem;
using namespace boost::algorithm;

enum fileTypes {IMAGES, VIDEOS, CASCADES};

bool checkExtention(const path &filePath, const std::vector<string> &extentions) {
	string fileExtention = filePath.extension().string();
	to_lower(fileExtention);

	for each (string extention in extentions)
		if (fileExtention == extention)
			return true;

	return false;
}

std::vector<path> getFiles(const path &thePath, fileTypes types) {

	string object_name;
	std::vector<string> extentions;

	switch (types)
	{
	case IMAGES:
		object_name = "images";
		extentions = { ".jpg", ".png", ".jpeg", ".pb", ".gif" };
		break;
	case VIDEOS:
		object_name = "videos";
		extentions = { ".avi" };
		break;
	case CASCADES:
		object_name = "cascades";
		extentions = { ".xml" };
		break;
	default:
		break;
	}

	std::vector<path> listOfFiles;

	if (exists(thePath)) {
		if (is_directory(thePath)) {
			for (auto& entry : boost::make_iterator_range(directory_iterator(thePath))) {
				path entryPath = entry.path();
				if (checkExtention(entryPath, extentions)) 
					listOfFiles.push_back(entryPath);
			}
		}
		else {
			if (checkExtention(thePath, extentions))
				listOfFiles.push_back(thePath);
		}
	}
	else {
		cout << "ERROR: " << thePath.string() << " is invalid path" << endl;
	}

	cout << listOfFiles.size() << " " << object_name << " in " << thePath << " found" << endl << endl;

	return listOfFiles;
}

#endif