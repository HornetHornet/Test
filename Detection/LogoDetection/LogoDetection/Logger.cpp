
#include "Logger.h"

/*members of logg*/

std::ofstream logg::clck;
std::ofstream logg::err;

time_t logg::start = clock();

std::ofstream logg::fout("logo_" + get_session_id() + ".log", std::ofstream::app);
Tee logg::tee(std::cout, fout);
TeeStream logg::tout(tee);

inline void logg::reset_clock() {
	start = clock();
}

inline void logg::write_clock(const std::string &name) {
	logg::clck << name << " took " << clock() - logg::start << std::endl;
	start = clock();
}

std::string logg::get_session_id() {

	using namespace std;
	using namespace std::chrono;

	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);

	struct tm local_tm;
	localtime_s(&local_tm, &tt);

	std::stringstream session_id;
	session_id << "D" << local_tm.tm_mday
		<< "_H" << local_tm.tm_hour
		<< "_M" << (local_tm.tm_min / 10) * 10;

	return session_id.str();
}