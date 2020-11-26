#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/filesystem/operations.hpp>


typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;


class Formatter{
	std::stringstream stream_;
	Formatter(const Formatter &);
	Formatter & operator = (Formatter &);
public:
	Formatter() {}
	~Formatter() {}

	template <typename Type>
	Formatter & operator << (const Type & value){
		stream_ << value;
		return *this;
	}

	std::string str() const         { return stream_.str(); }
	operator std::string () const   { return stream_.str(); }

	enum ConvertToString{
		to_str
	};
	std::string operator >> (ConvertToString) { return stream_.str(); }
};


#define expect(X) if (!X) throw std::runtime_error(\
	Formatter() << __FILE__ << ":" << __LINE__ << " " << #X >> Formatter::to_str);


class Logger {
	std::ofstream log_stream;

public:

	Logger(const std::string & id);

	TeeStream state_stream;
	TeeStream err_stream;
};

void init_logger(const std::string & id);
std::shared_ptr<Logger> get_logger();

#define log_state (get_logger() ? get_logger()->state_stream : std::cout)
#define log_err (get_logger() ? get_logger()->err_stream : std::cerr) << "ERROR: "

#endif


