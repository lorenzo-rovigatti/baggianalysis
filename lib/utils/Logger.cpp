/*
 * Logger.cpp
 *
 *  Created on: 28 gen 2021
 *      Author: lorenzo
 */

#include "Logger.h"

namespace ba {

std::shared_ptr<spdlog::logger> Logger::_spd_logger = nullptr;

std::shared_ptr<spdlog::logger> Logger::logger() {
	if(Logger::_spd_logger == nullptr) {
		Logger::_spd_logger = spdlog::stderr_logger_st("ba_logger");
	}

	return Logger::_spd_logger;
}

} /* namespace ba */
