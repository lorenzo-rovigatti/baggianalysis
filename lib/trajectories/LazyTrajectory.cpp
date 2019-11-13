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

void LazyTrajectory::initialise_from_filelist(std::vector<std::string> filelist) {
	_type = FOLDER;

	_files = filelist;
	_current_file = _files.begin();

	BOOST_LOG_TRIVIAL(info)<<"Found " << _files.size() << " files";
}

shared_ptr<System> LazyTrajectory::next_frame() {
	std::shared_ptr<System> new_system = nullptr;

	if(_type == TRAJECTORY_FILE) {
		new_system = _parser->parse_stream(_trajectory_file);
		if(new_system != nullptr) {
			new_system = _filtered_system(new_system);
		}
	}
	else if(_type == FOLDER) {
		if(_current_file != _files.end()) {
			new_system = _parser->parse_file(*_current_file);
			new_system = _filtered_system(new_system);

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
		_trajectory_file.clear();
		_trajectory_file.seekg(0, ios::beg);
	}
	else if(_type == FOLDER) {
		_current_file = _files.begin();
	}
}

#ifdef PYTHON_BINDINGS

void export_LazyTrajectory(py::module &m) {
	py::class_<LazyTrajectory, BaseTrajectory, std::shared_ptr<LazyTrajectory>> parser(m, "LazyTrajectory");

	parser
		.def(py::init<shared_ptr<BaseParser>>());
}

#endif

} /* namespace ba */
