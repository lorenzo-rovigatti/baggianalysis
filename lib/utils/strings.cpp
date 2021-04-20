/*
 * strings.cpp
 *
 *  Created on: Feb 28, 2020
 *      Author: lorenzo
 */

#include "strings.h"

namespace ba {

namespace utils {

// taken from https://www.bfilipek.com/2018/07/string-view-perf-followup.html
std::vector<std::string> split(const std::string &str, const std::string &delims) {
	std::vector<std::string> output;
	auto first = std::cbegin(str);

	while(first != std::cend(str)) {
		const auto second = std::find_first_of(first, std::cend(str), std::cbegin(delims), std::cend(delims));

		if(first != second) {
			output.emplace_back(first, second);
		}

		if(second == std::cend(str)) {
			break;
		}

		first = std::next(second);
	}

	if(output.size() == 0) {
		output.emplace_back("");
	}

	return output;
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

vec3 vector_from_line(std::string &line) {
	auto split_line = utils::split(line);
	vec3 v;

	try {
		v = vec3(
				lexical_cast<double>(utils::trim_copy(split_line[0])),
				lexical_cast<double>(utils::trim_copy(split_line[1])),
				lexical_cast<double>(utils::trim_copy(split_line[2]))
				);
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The line '{}' cannot be converted to a three-dimensional vector", line);
		throw std::runtime_error(error);
	}

	return v;
}

std::string trim_copy(std::string source) {
	trim(source);
	return source;
}

}

}
