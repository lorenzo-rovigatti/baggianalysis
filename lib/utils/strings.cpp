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

	std::size_t prev = 0, pos;
	while((pos = line.find_first_of(separators, prev)) != std::string::npos) {
		if(pos > prev) {
			split.push_back(line.substr(prev, pos - prev));
		}
		prev = pos + 1;
	}

	if(prev < line.length()) {
		split.push_back(line.substr(prev, std::string::npos));
	}

	if(split.size() == 0) {
		split.push_back("");
	}

	return split;
}

bool starts_with(const std::string &value, std::string beginning) {
	if(beginning.size() > value.size()) {
		return false;
	}
	return std::equal(beginning.begin(), beginning.end(), value.begin());
}

bool ends_with(const std::string &value, std::string ending) {
	if(ending.size() > value.size()) {
		return false;
	}
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool contains(const std::string &value, std::string query) {
	return value.find(query) != std::string::npos;
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
