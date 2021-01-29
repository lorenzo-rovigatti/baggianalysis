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

void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

std::string trim_copy(std::string source) {
	trim(source);
	return source;
}

}

}
