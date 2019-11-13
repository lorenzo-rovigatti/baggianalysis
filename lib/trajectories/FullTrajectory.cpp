/*
 * Trajectory.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "FullTrajectory.h"

#include "../System.h"
#include "../utils/filesystem.h"

#include <boost/filesystem.hpp>
#include <fstream>

namespace ba {

namespace bfs = boost::filesystem;
using namespace std;

FullTrajectory::FullTrajectory(shared_ptr<BaseParser> parser) :
				BaseTrajectory(parser) {

}

FullTrajectory::~FullTrajectory() {

}

void FullTrajectory::add_filter(shared_ptr<BaseFilter> filter) {
	if(frames.size() > 0) {
		BOOST_LOG_TRIVIAL(warning)<<"Adding filters to trajectories that have been already initialised does not make much sense...";
	}

	BaseTrajectory::add_filter(filter);
}

void FullTrajectory::initialise_from_trajectory_file(string trajectory_file) {
	ifstream trajectory(trajectory_file);

	if(!trajectory.good()) {
		string error = boost::str(boost::format("Unreadable trajectory file '%s'") % trajectory_file);
		throw runtime_error(error);
	}

	bool done = false;
	while(!done) {
		auto new_system = _parser->make_system(trajectory);
		if(new_system == nullptr) {
			done = true;
		}
		else {
			frames.push_back(_filtered_system(new_system));
		}
	}

	trajectory.close();

	_current_system = frames.begin();

	if(frames.size() == 1) {
		BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frame";
	}
	else {
		BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frames";
	}
}

void FullTrajectory::initialise_from_filelist(std::vector<std::string> filelist) {
	uint N_files = filelist.size();

	for(auto f : filelist) {
		auto new_system = _parser->make_system(f);

		if(new_system == nullptr) {
			string error = boost::str(boost::format("The '%s' configuration is either empty or invalid") % f);
			throw runtime_error(error);
		}
		frames.push_back(_filtered_system(new_system));
		uint N_frames = frames.size();
		if(N_frames > 10 && N_frames % (N_files / 10) == 0) BOOST_LOG_TRIVIAL(info)<< "Parsed " << N_frames * 100 / N_files << "% of the configurations (" << N_frames << "/" << N_files << ")";
	}

	// we sort the frames according to their timestep
	auto sorting_function = [](const shared_ptr<System> &s1, const shared_ptr<System> &s2) -> bool {
		return (s1->time < s2->time);
	};
	stable_sort(frames.begin(), frames.end(), sorting_function);

	_current_system = frames.begin();

	BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frames";
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
	py::class_<FullTrajectory, BaseTrajectory, std::shared_ptr<FullTrajectory>> parser(m, "FullTrajectory");

	parser
		.def(py::init<shared_ptr<BaseParser>>()).
		def_readonly("frames", &FullTrajectory::frames);
}

#endif

} /* namespace ba */
