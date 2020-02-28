/*
 * strings.cpp
 *
 *  Created on: Feb 28, 2020
 *      Author: lorenzo
 */

#include "strings.h"

namespace ba {

namespace utils {

std::vector<std::string> split(std::string line, std::string separators) {
	std::vector<std::string> split;
	boost::split(split, line, boost::is_any_of(separators), boost::algorithm::token_compress_on);

	return split;
}

std::string trim(std::string source) {
	return boost::trim_copy(source);
}

}

}
