#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;

class logg {
	static time_t start;
public:
	static std::ofstream logg::clck;
	static std::ofstream logg::err;
	static std::ofstream logg::fout;

	static Tee logg::tee;
	static TeeStream logg::tout;

	static void reset_clock();
	static void write_clock(const std::string name);
	static std::string get_session_id();
};

#endif


