/*
 * LazyTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "LazyTrajectory.h"

#include "../utils/filesystem.h"

#include <boost/filesystem.hpp>

namespace ba {

namespace bfs = boost::filesystem;
using namespace std;

LazyTrajectory::LazyTrajectory(shared_ptr<BaseParser> parser) :
				BaseTrajectory(parser),
				_type(UNINITIALISED) {

}

LazyTrajectory::~LazyTrajectory() {

}

void LazyTrajectory::initialise_from_trajectory_file(string trajectory_file) {
	_type = TRAJECTORY_FILE;

	_trajectory_file.open(trajectory_file);

	if(!_trajectory_file.good()) {
		string error = boost::str(boost::format("Unreadable trajectory file '%s'") % trajectory_file);
		throw runtime_error(error);
	}
}

void LazyTrajectory::initialise_from_folder(string folder, string pattern) {
	_type = FOLDER;

	bfs::path path(folder);

	if(!bfs::exists(path)) {
		string error = boost::str(boost::format("The '%s' folder does not exist") % folder);
		throw runtime_error(error);
	}

	if(!bfs::is_directory(path)) {
		string error = boost::str(boost::format("'%s' is not a folder") % folder);
		throw runtime_error(error);
	}

	bfs::path tot_path = bfs::path(folder) / bfs::path(pattern);
	_files = utils::glob(tot_path.string(), true);
	_current_file = _files.begin();

	BOOST_LOG_TRIVIAL(info)<<"Found " << _files.size() << " files";
}

shared_ptr<System> LazyTrajectory::next_frame() {
	std::shared_ptr<System> new_system = nullptr;

	if(_type == TRAJECTORY_FILE) {
		new_system = _parser->parse(_trajectory_file);
		if(new_system == nullptr) {
			_trajectory_file.close();
		}
		else {
			for(auto filter : _filters) {
				new_system = filter->filter(new_system);
			}
		}
	}
	else if(_type == FOLDER) {
		if(_current_file != _files.end()) {
			ifstream conf_file(*_current_file);
			new_system = _parser->parse(conf_file);
			conf_file.close();

			for(auto filter : _filters) {
				new_system = filter->filter(new_system);
			}

			_current_file++;
		}
	}
	else {
		string error = boost::str(boost::format("Cannot access the next frame: trajectory uninitialised"));
		throw runtime_error(error);
	}

	return new_system;
}

void LazyTrajectory::reset() {
	if(_type == TRAJECTORY_FILE) {
		_trajectory_file.seekg(0);
	}
	else if(_type == FOLDER) {
		_current_file = _files.begin();
	}
}

} /* namespace ba */
