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
#include <fast_double_parser/fast_double_parser.h>

namespace ba {

namespace utils {

/**
 * Split a string into tokens.
 *
 * @param line
 * @param separators list of separators that will be used to split the string
 * @return a vector of strings containing the tokens
 */
std::vector<std::string> split(const std::string &str, const std::string &delims=" ");

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
 * Cast the given string to a numeric type. Trim the string before attempting to cast it.
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

template<>
inline double lexical_cast(std::string source) {
	double result;

	const char *endptr = fast_double_parser::parse_number(source.c_str(), &result);

	if(endptr == nullptr) {
		throw bad_lexical_cast();
	}

	return result;
}

template<>
inline int lexical_cast(std::string source) {
	int result;

	try {
		result = std::stoi(source);
	}
	catch (std::invalid_argument &) {
		throw bad_lexical_cast();
	}
	catch (std::out_of_range &) {
		throw bad_lexical_cast();
	}

	return result;
}

}

}

#endif /* UTILS_STRINGS_H_ */
