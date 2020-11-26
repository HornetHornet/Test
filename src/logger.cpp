#include "logger.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/filesystem/operations.hpp>


Logger::Logger(const std::string & id)
: state_stream(Tee(std::cout, log_stream)), err_stream(Tee(std::cerr, log_stream))
{
	namespace bfs = boost::filesystem;
	bfs::path log_parent_path = "logs/";

	if(!bfs::exists(log_parent_path))
		bfs::create_directories(log_parent_path);
	expect(bfs::exists(log_parent_path));

	bfs::path log_path = log_parent_path / (id + ".log");

	log_stream.open(log_path.string());
	expect(log_stream.is_open());
}


std::shared_ptr<Logger> logger_p;

void init_logger(const std::string & id){
	logger_p.reset(new Logger(id));
}

std::shared_ptr<Logger> get_logger(){
	return logger_p;
}


