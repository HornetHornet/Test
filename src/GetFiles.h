#pragma once

#ifndef GETFILES_H
#define GETFILES_H

#include <iostream>
#include <stdio.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 

using namespace boost::filesystem;
using namespace boost::algorithm;

enum fileTypes {IMAGES, VIDEOS, CASCADES};

bool checkExtention(const path &filePath, const std::vector<std::string> &extentions) {

	if (!filePath.has_extension())
		return false;

	std::string fileExtention = filePath.extension().string();
	to_lower(fileExtention);

	for(const auto & extention : extentions)
		if (fileExtention == extention)
			return true;

	return false;
}

// if given directory returns list of paths to files of requested type
// if given path to a file returns it if the file's type is the one that was requested
std::vector<path> getFiles(const path &thePath, fileTypes types, bool recusively = false) {

	std::vector<path> listOfFiles;

	if (!exists(thePath)) {
		std::cerr << "ERROR: " << thePath.string() << " is invalid path" << std::endl;
		return listOfFiles;
	}
	
	std::string object_name;
	std::vector<std::string> extentions;

	switch (types) {
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
	}

	if (!is_directory(thePath)) {
		if (checkExtention(thePath, extentions))
			listOfFiles.push_back(thePath);
	}
	else {
		if (recusively) {
			for (auto& entry : boost::make_iterator_range(recursive_directory_iterator(thePath))) {
				path entryPath = entry.path();
				if (checkExtention(entryPath, extentions))
					listOfFiles.push_back(entryPath);
			}
		}
		else {
			for (auto& entry : boost::make_iterator_range(directory_iterator(thePath))) {
				path entryPath = entry.path();
				if (checkExtention(entryPath, extentions))
					listOfFiles.push_back(entryPath);
			}
		}
	}
		
	std::cout << listOfFiles.size() << " " << object_name << " in " << thePath << " found" << std::endl;

	return listOfFiles;
}

#endif