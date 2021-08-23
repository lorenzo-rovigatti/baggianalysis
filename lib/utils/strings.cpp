/*
 * strings.cpp
 *
 *  Created on: Feb 28, 2020
 *      Author: lorenzo
 */

#include "strings.h"

#include <bitset>

namespace ba {

namespace utils {

std::vector<std::string> split(const std::string &str, const std::string &delims) {
	std::vector<std::string> output;

	std::bitset<255> b_delims;
	char const *d = delims.data();
	while(*d) {
		unsigned char code = *d++;
		b_delims[code] = true;
	}
	typedef std::string::const_iterator iter;
	iter beg;
	bool in_token = false;
	for(std::string::const_iterator it = str.begin(), end = str.end(); it != end; ++it) {
		if(b_delims[*it]) {
			if(in_token) {
				output.emplace_back(beg, it);
				in_token = false;
			}
		}
		else if(!in_token) {
			beg = it;
			in_token = true;
		}
	}
	if(in_token) {
		output.emplace_back(beg, str.end());
	}

	if(output.size() == 0) {
		output.push_back("");
	}

	return output;
}

//std::vector<std::string> split(const std::string &str, const std::string &delims) {
//	std::vector<std::string> output;
//
//	const char *ptr = str.c_str();
//	while(ptr) {
//		auto base = ptr;
//		ptr = std::strpbrk(ptr, delims.c_str());
//		if(ptr) {
//			output.emplace_back(base, ptr - base);
//			ptr++;
//		}
//		else {
//			output.emplace_back(base);
//		}
//	}
//
//	if(output.size() == 0) {
//		output.push_back("");
//	}
//
//	return output;
//}

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
