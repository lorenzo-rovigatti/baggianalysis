/*
 * filesystem.h
 *
 *  Created on: Feb 1, 2019
 *      Author: lorenzo
 */

#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include "../defs.h"

#include <glob.h>

namespace ba {

namespace utils {

/**
 * @brief Thin C++ wrapper around C's glob function
 *
 * The second argument optionally sorts the entries.
 *
 * @param pattern matching pattern
 * @param natural_sorting sort the entries according to natural sorting criteria
 * @return the list of entries matching the given pattern
 */
std::vector<std::string> glob(const std::string& pattern, bool natural_sorting=false);

std::string join_paths(std::string path1, std::string path2);

bool exists(std::string path);

bool is_directory(std::string path);

}

}

#endif /* UTILS_FILESYSTEM_H_ */
