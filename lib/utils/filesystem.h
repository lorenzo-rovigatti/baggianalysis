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

std::vector<std::string> glob(const std::string& path, bool natural_sorting);

}

}

#endif /* UTILS_FILESYSTEM_H_ */
