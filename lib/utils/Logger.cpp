/*
 * Logger.cpp
 *
 *  Created on: 28 gen 2021
 *      Author: lorenzo
 */

#include "Logger.h"

namespace ba {

std::shared_ptr<spdlog::logger> Logger::_spd_logger = nullptr;
Logger::LogMode Logger::_log_mode = Logger::LogMode::STDERR;
std::string Logger::_log_file = "ba_log.txt";

std::shared_ptr<spdlog::logger> Logger::logger() {
	if(Logger::_spd_logger == nullptr) {
		set_logging_mode(_log_mode, _log_file);
	}

	return Logger::_spd_logger;
}

void Logger::set_logging_mode(LogMode mode, const std::string &file_path) {
    _log_mode = mode;
    _log_file = file_path;

    // make sure that the ba_logger does not exist
    spdlog::drop("ba_logger");

    switch (_log_mode) {
        case LogMode::STDERR:
            _spd_logger = spdlog::stderr_logger_st("ba_logger");
            break;
        case LogMode::FILE:
            _spd_logger = spdlog::basic_logger_st("ba_logger", _log_file);
            break;
        case LogMode::SILENT:
            _spd_logger = spdlog::null_logger_st("ba_logger");
            break;
    }
}

} /* namespace ba */
