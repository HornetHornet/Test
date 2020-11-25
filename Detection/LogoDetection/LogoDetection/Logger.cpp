
#include <iomanip>
#include "Logger.h"

/*members of logg*/

//std::ofstream logg::clck("clock.log", std::ofstream::app);
std::ofstream logg::clck;
std::ofstream logg::err;

time_t logg::start = clock();

std::ofstream logg::fout("logo_" + get_session_id() + ".log", std::ofstream::app);
Tee logg::tee(std::cout, fout);
TeeStream logg::tout(tee);

void logg::reset_clock() {
	start = clock();
}

void logg::write_clock(const std::string name) {
	logg::clck << name << " took " << clock() - logg::start << std::endl;
	start = clock();
}

std::string logg::get_session_id() {

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);

	std::stringstream session_id;
	session_id << std::put_time(std::localtime(&tt), "%Y-%m-%d-%H:%M:%S");

//	struct tm local_tm;
//	localtime_s(&local_tm, &tt);
//
//
//	session_id << "D" << local_tm.tm_mday
//		<< "_H" << local_tm.tm_hour
//		<< "_M" << (local_tm.tm_min / 10) * 10;

	return session_id.str();
}