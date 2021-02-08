/*
 * LazyTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "LazyTrajectory.h"

#include "../utils/filesystem.h"

namespace ba {

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
		string error = fmt::format("Unreadable trajectory file '{}'", trajectory_file);
		throw runtime_error(error);
	}
}

void LazyTrajectory::initialise_from_filelist(std::vector<std::string> filelist) {
	if(filelist.size() == 0) {
		string error = fmt::format("Trying to initialise a trajectory from an empty filelist");
		throw runtime_error(error);
	}

	_type = FOLDER;

	_files = filelist;
	_current_file = _files.begin();

	BA_INFO("Found {} files", _files.size());
}

shared_ptr<System> LazyTrajectory::next_frame() {
	std::shared_ptr<System> new_system = nullptr;

	if(_type == TRAJECTORY_FILE) {
		new_system = _parser->make_system(_trajectory_file);
		if(new_system != nullptr) {
			new_system = _filtered_system(new_system);
		}
	}
	else if(_type == FOLDER) {
		if(_current_file != _files.end()) {
			new_system = _parser->make_system(*_current_file);
			new_system = _filtered_system(new_system);

			_current_file++;
		}
	}
	else {
		string error = fmt::format("Cannot access the next frame: trajectory uninitialised");
		throw runtime_error(error);
	}

	if(new_system != nullptr && !_include_system(new_system)) {
		return next_frame();
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
	py::class_<LazyTrajectory, BaseTrajectory, std::shared_ptr<LazyTrajectory>> parser(m, "LazyTrajectory", R"pbdoc(
        A trajectory that does not load up all the frames in memory at once but parses them one after the other.

        Use it if you need to analyse large trajectories that could fill up all the available memory. Be careful though:
        the file(s) on the disk will be read every time you loop over the trajectory. If you need to do it more than
        once it may be worth using a :class:`FullTrajectory`. 
    )pbdoc");

	parser.def(py::init<shared_ptr<BaseParser>>(), "The constructor takes a parser as its only parameter.");
}

#endif

} /* namespace ba */
