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
		auto new_system = _parser->parse(trajectory);
		if(new_system == nullptr) {
			done = true;
		}
		else {
			for(auto filter : _filters) {
				new_system = filter->filter(new_system);
			}
			frames.push_back(new_system);
		}
	}

	trajectory.close();

	_current_system = frames.begin();

	BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frames";
}

void FullTrajectory::initialise_from_folder(string folder, string pattern) {
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
	vector<string> files = utils::glob(tot_path.string(), false);

	uint N_files = files.size();

	for(auto f : files) {
		ifstream conf_file(f);
		auto new_system = _parser->parse(conf_file);
		conf_file.close();

		if(new_system == nullptr) {
			string error = boost::str(boost::format("The '%s' configuration is either empty or invalid") % f);
			throw runtime_error(error);
		}
		for(auto filter : _filters) {
			new_system = filter->filter(new_system);
		}
		frames.push_back(new_system);
		uint N_frames = frames.size();
		if(N_frames > 10 && N_frames % (N_files / 10) == 0) BOOST_LOG_TRIVIAL(info)<< "Parsed " << N_frames * 100 / N_files << "% of the configurations (" << N_frames << "/" << N_files << ")";
	}

	// we sort the frames according to their timestep
	auto sorting_function = [pattern](const shared_ptr<System> &s1, const shared_ptr<System> &s2) -> bool {
		return (s1->time < s2->time);
	};
	sort(frames.begin(), frames.end(), sorting_function);

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

}
/* namespace ba */
