/*
 * Logger.h
 *
 *  Created on: 28 gen 2021
 *      Author: lorenzo
 */

#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#define BA_CRITICAL Logger::critical
#define BA_ERROR Logger::logger()->error
#define BA_WARNING Logger::logger()->warn
#define BA_INFO Logger::logger()->info
#define BA_DEBUG Logger::logger()->debug

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <memory>

namespace ba {

class Logger {
public:
	Logger() = delete;
	virtual ~Logger() = delete;

	static std::shared_ptr<spdlog::logger> logger();

	template<typename FormatString, typename... Args>
	static void critical(const FormatString &fmt, Args&&...args) {
		Logger::logger()->critical(fmt, std::forward<Args>(args)...);
		exit(1);
	}

private:
	static std::shared_ptr<spdlog::logger> _spd_logger;
};

} /* namespace ba */

#endif /* UTILS_LOGGER_H_ */
