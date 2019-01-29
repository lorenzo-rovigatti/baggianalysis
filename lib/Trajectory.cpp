/*
 * Trajectory.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "Trajectory.h"
#include "parsers/OxDNAParser.h"
#include "System.h"

#include <fstream>

namespace ba {

using namespace std;

Trajectory::Trajectory() {

}

Trajectory::~Trajectory() {

}

void Trajectory::setTopologyFile(std::string topology_file) {
	_topology_file = topology_file;
}

void Trajectory::setTrajectoryFile(std::string trajectory_file) {
	_trajectory_file = trajectory_file;
}

void Trajectory::initialise() {
	if(_topology_file == "") {
		throw std::runtime_error("Uninitialised topology file");
	}

	ifstream topology(_topology_file);

	if(!topology.good()) {
		std::string error = boost::str(boost::format("Topology file '%s' not found") % _topology_file);
		throw std::runtime_error(error);
	}

	if(_trajectory_file == "") {
		throw std::runtime_error("Uninitialised trajectory file");
	}

	ifstream trajectory(_trajectory_file);

	if(!trajectory.good()) {
		std::string error = boost::str(boost::format("Trajectory file '%s' not found") % _trajectory_file);
		throw std::runtime_error(error);
	}

	bool done = false;
	while(!done) {
		auto new_system = OxDNAParser::parse(topology, trajectory);
		if(new_system == nullptr) {
			done = true;
		}
		else {
			frames.push_back(new_system);
		}
	}

	topology.close();
	trajectory.close();

	BOOST_LOG_TRIVIAL(info) << "Loaded " << frames.size() << " frames";
}

} /* namespace ba */
