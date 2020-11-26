#pragma once

#ifndef GETFILES_H
#define GETFILES_H

#include <iostream>
#include <stdio.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include "logger.hpp"

enum fileTypes {IMAGES, VIDEOS, CASCADES};

// if given directory returns list of paths to files of requested type
// if given path to a file returns it if the file's type is the one that was requested
std::vector<boost::filesystem::path> list_files(const std::string &parent_path, fileTypes types) {
	namespace bfs = boost::filesystem;

	auto check_extention = [](
			const bfs::path &file_path,
			const std::vector<std::string> &extentions)
	{
		if (!bfs::path(file_path).has_extension())
			return false;

		std::string fileExtention = file_path.extension().string();
		boost::algorithm::to_lower(fileExtention);

		for(const auto & extention : extentions)
			if (fileExtention == extention)
				return true;

		return false;
	};


	std::vector<bfs::path> list_of_files;

	if (!bfs::exists(parent_path)) {
		log_err << "ERROR: " << parent_path << " is invalid path" << std::endl;
		return list_of_files;
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

	if (!bfs::is_directory(parent_path)) {
		if (check_extention(parent_path, extentions))
			list_of_files.push_back(parent_path);
	}
	else {
		for (auto& entry_path : boost::make_iterator_range(bfs::recursive_directory_iterator(parent_path))) {
			if (check_extention(entry_path, extentions))
				list_of_files.push_back(entry_path.path().string());
		}
	}
		
	log_state << list_of_files.size() << " " << object_name << " in " << parent_path << " found" << std::endl;

	return list_of_files;
}

#endif