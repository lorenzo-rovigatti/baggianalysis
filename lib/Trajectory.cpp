/*
 * Trajectory.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "Trajectory.h"
#include "parsers/OxDNAParser.h"
#include "System.h"

#include <boost/filesystem.hpp>
#include <fstream>

namespace ba {

namespace bfs = boost::filesystem;
using namespace std;

Trajectory::Trajectory(shared_ptr<BaseParser> parser) :
				_parser(parser) {

}

Trajectory::~Trajectory() {

}

void Trajectory::add_filter(std::shared_ptr<BaseFilter> filter) {
	if(frames.size() > 0) {
		BOOST_LOG_TRIVIAL(warning)<<"Adding filters to trajectories that have been already initialised does not make much sense...";
	}

	_filters.push_back(filter);
}

void Trajectory::initialise_from_trajectory_file(std::string trajectory_file) {
	ifstream trajectory(trajectory_file);

	if(!trajectory.good()) {
		std::string error = boost::str(boost::format("Unreadable trajectory file '%s'") % trajectory_file);
		throw std::runtime_error(error);
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

	BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frames";
}

void Trajectory::initialise_from_folder(std::string folder, std::string prefix) {
	bfs::path path(folder);

	if(!bfs::exists(path)) {
		std::string error = boost::str(boost::format("The '%s' folder does not exist") % folder);
		throw std::runtime_error(error);
	}

	if(!bfs::is_directory(path)) {
		std::string error = boost::str(boost::format("'%s' is not a folder") % folder);
		throw std::runtime_error(error);
	}

	std::vector<std::string> files;

	for(auto &entry : bfs::directory_iterator(path)) {
		bool starts_with_prefix = (entry.path().filename().string().rfind(prefix, 0) == 0);
		if(bfs::is_regular_file(entry) && starts_with_prefix) {
			files.push_back(entry.path().string());
		}
	}

	std::sort(files.begin(), files.end());

	for(auto f : files) {
		ifstream conf_file(f);
		auto new_system = _parser->parse(conf_file);
		conf_file.close();

		if(new_system == nullptr) {
			std::string error = boost::str(boost::format("The '%s' configuration is either empty or invalid") % f);
			throw std::runtime_error(error);
		}
		for(auto filter : _filters) {
			new_system = filter->filter(new_system);
		}
		frames.push_back(new_system);
	}

	BOOST_LOG_TRIVIAL(info)<<"Loaded " << frames.size() << " frames";
}

}
/* namespace ba */
