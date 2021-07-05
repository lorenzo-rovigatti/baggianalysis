/*
 * test_feature.cpp
 *
 *  Created on: Sep 4, 2019
 *      Author: lorenzo
 */


#include <trajectories/FullTrajectory.h>
#include <trajectories/LazyTrajectory.h>
#include <neighbour_finders/SANNFinder.h>
#include <topology/Topology.h>
#include <topology/TopologyParsers.h>
#include "../lib/parsers/LAMMPSDataFileParser.h"
#include "../lib/parsers/LAMMPSDumpParser.h"
#include "../lib/parsers/OxDNAParser.h"

int main(int argc, char *argv[]) {
//	std::ios::sync_with_stdio(false);

	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " topology trajectory" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::Topology> topology = ba::Topology::make_topology_from_file("links.dat", ba::parse_polymer_bondfile);
	std::shared_ptr<ba::OxDNAParser> parser = std::make_shared<ba::OxDNAParser>(argv[1]);
	parser->set_topology(topology);

	std::shared_ptr<ba::FullTrajectory> trajectory = std::make_shared<ba::FullTrajectory>(parser);
	trajectory->initialise_from_trajectory_file(argv[2]);

	return 0;
}

