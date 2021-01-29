/*
 * Trajectory.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "FullTrajectory.h"

#include "../System.h"
#include "../utils/filesystem.h"

#include <fstream>

namespace ba {

using namespace std;

FullTrajectory::FullTrajectory(shared_ptr<BaseParser> parser) :
				BaseTrajectory(parser) {

}

FullTrajectory::~FullTrajectory() {

}

void FullTrajectory::add_filter(shared_ptr<BaseFilter> filter) {
	if(frames.size() > 0) {
		BA_WARNING("Adding filters to trajectories that have been already initialised may result in unwanted behaviours");
	}

	BaseTrajectory::add_filter(filter);
}

void FullTrajectory::initialise_from_trajectory_file(string trajectory_file) {
	ifstream trajectory(trajectory_file);

	if(!trajectory.good()) {
		string error = fmt::format("Unreadable trajectory file '{}'", trajectory_file);
		throw runtime_error(error);
	}

	bool done = false;
	while(!done) {
		auto new_system = _parser->make_system(trajectory);
		if(new_system == nullptr) {
			done = true;
		}
		else {
			if(_include_system(new_system)) {
				frames.push_back(_filtered_system(new_system));
			}
		}
	}

	trajectory.close();

	_current_system = frames.begin();

	if(frames.size() == 1) {
		BA_INFO("Loaded 1 frame");
	}
	else {
		BA_INFO("Loaded {} frames", frames.size());
	}
}

void FullTrajectory::initialise_from_filelist(std::vector<std::string> filelist) {
	uint N_files = filelist.size();

	uint parsed_files = 0;
	for(auto f : filelist) {
		auto new_system = _parser->make_system(f);
		parsed_files++;

		if(new_system == nullptr) {
			string error = fmt::format("The '{}' configuration is either empty or invalid", f);
			throw runtime_error(error);
		}

		if(_include_system(new_system)) {
			frames.push_back(_filtered_system(new_system));
		}

		if(parsed_files > 10 && parsed_files % (N_files / 10) == 0) {
			BA_INFO("Parsed {}% of the configurations ({}/{})", parsed_files * 100 / N_files, parsed_files, N_files);
		}
	}

	// we sort the frames according to their timestep
	auto sorting_function = [](const shared_ptr<System> &s1, const shared_ptr<System> &s2) -> bool {
		return (s1->time < s2->time);
	};
	stable_sort(frames.begin(), frames.end(), sorting_function);

	_current_system = frames.begin();

	if(frames.size() == 1) {
		BA_INFO("Loaded 1 frame");
	}
	else {
		BA_INFO("Loaded {} frames", frames.size());
	}
}

shared_ptr<System> FullTrajectory::next_frame() {
	if(_current_system != frames.end()) {
		auto syst = *_current_system;
		_current_system++;
		return syst;
	}
	else {
		return nullptr;
	}
}

void FullTrajectory::reset() {
	_current_system = frames.begin();
}

#ifdef PYTHON_BINDINGS

void export_FullTrajectory(py::module &m) {
	py::class_<FullTrajectory, BaseTrajectory, std::shared_ptr<FullTrajectory>> parser(m, "FullTrajectory", R"pbdoc(
        A trajectory that builds and loads in memory all the frames at once.

        This trajectory is probably the one you should be using unless the memory footprint of the files that should be
        parsed becomes too large, in which case you should use a :class:`LazyTrajectory`.
    )pbdoc");

	parser.def(py::init<shared_ptr<BaseParser>>(), "The constructor takes a parser as its only parameter.");
	parser.def_readonly("frames", &FullTrajectory::frames, "The read-only list of :class:`System` objects making up the trajectory.");
}

#endif

} /* namespace ba */
