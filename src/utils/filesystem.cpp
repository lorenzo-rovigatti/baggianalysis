/*
 * filesystem.cpp
 *
 *  Created on: Feb 1, 2019
 *      Author: lorenzo
 */

#include "filesystem.h"

#include "natural_sort.h"

#include <sys/stat.h>
#include <cerrno>

namespace ba {

namespace utils {

using namespace std;

std::vector<std::string> glob(const std::string &path, bool natural_sorting) {
	glob_t glob_result;
	glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);

	vector<string> ret;
	for(unsigned int i = 0; i < glob_result.gl_pathc; i++) {
		ret.push_back(string(glob_result.gl_pathv[i]));
	}
	globfree(&glob_result);

	if(natural_sorting) {
		SI::natural::sort(ret);
	}

	return ret;
}

std::string join_paths(std::string path1, std::string path2) {
	return path1 + "/" + path2;
}

bool exists(std::string path) {
	struct stat info;

	int stat_ret = stat(path.c_str(), &info);
	return (!stat_ret);
}

bool is_directory(std::string path) {
	struct stat info;

	int stat_ret = stat(path.c_str(), &info);
	if(stat_ret) {
		return false;
	}

	return S_ISDIR(info.st_mode);
}

}

}
