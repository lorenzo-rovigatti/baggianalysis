/*
 * strings.h
 *
 *  Created on: Feb 28, 2020
 *      Author: lorenzo
 */

#ifndef UTILS_STRINGS_H_
#define UTILS_STRINGS_H_

#include "../defs.h"

#include <sstream>

namespace ba {

namespace utils {

/**
 * Split a string into tokens.
 *
 * @param line
 * @param separators list of separators that will be used to split the string
 * @return a vector of strings containing the tokens
 */
std::vector<std::string> split(std::string line, std::string separators = " ");

// taken from https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c/2072890
bool starts_with(const std::string &value, std::string beginning);
bool ends_with(const std::string &value, std::string ending);
bool contains(const std::string &value, std::string query);

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

vec3 vector_from_line(std::string &line);

/**
 * Return a copy of the given string, without trailing and leading spaces
 *
 * @param source
 * @return a new string
 */
std::string trim_copy(std::string source);

class bad_lexical_cast: public std::exception {
	using std::exception::exception;
};

/**
 * Cast the given string to a numeric type. Trims the string before attempting to cast it.
 *
 * @param source
 * @return
 */
template<typename T>
T lexical_cast(std::string source) {
	T var;
	std::istringstream iss;
	iss.str(trim_copy(source));
	iss >> var;

	if(iss.fail() || iss.bad()) {
		throw bad_lexical_cast();
	}

	return var;
}

}

}

#endif /* UTILS_STRINGS_H_ */
