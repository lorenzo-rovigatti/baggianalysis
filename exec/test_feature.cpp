/*
 * test_feature.cpp
 *
 *  Created on: Sep 4, 2019
 *      Author: lorenzo
 */


#include <trajectories/FullTrajectory.h>
#include <trajectories/LazyTrajectory.h>
#include <neighbour_finders/SANNFinder.h>
#include <topology/TopologyParsers.h>
#include "../lib/parsers/LAMMPSDataFileParser.h"
#include "../lib/parsers/LAMMPSDumpParser.h"

int main(int argc, char *argv[]) {
	std::ios::sync_with_stdio(false);

	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " data_file folder" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::LAMMPSDumpParser> parser = std::make_shared<ba::LAMMPSDumpParser>(argv[1], "full");
	parser->use_topology(false);
	std::shared_ptr<ba::FullTrajectory> trajectory = std::make_shared<ba::FullTrajectory>(parser);
	trajectory->initialise_from_folder(argv[2], "pvdf_*", true);

	return 0;
}

//int main(int argc, char *argv[]) {
//	if(argc < 4) {
//		std::cerr << "Usage is " << argv[0] << " topology configuration bond_conf" << std::endl;
//		return 1;
//	}
//
//	std::shared_ptr<ba::BaseParser> parser(new ba::OxDNAParser(argv[1]));
//	auto topology = ba::Topology::make_topology_from_file(argv[3], ba::parse_microgel_bondfile);
//	parser->set_topology(topology);
//
//	std::shared_ptr<ba::FullTrajectory> trajectory(new ba::FullTrajectory(parser));
//	trajectory->initialise_from_trajectory_file(argv[2]);
//
//	std::shared_ptr<ba::SANNFinder> finder(new ba::SANNFinder(2.8, ba::SANNFinder::SYMMETRISE_BY_REMOVING));
//
//	auto syst = trajectory->next_frame();
//	finder->set_neighbours(syst->particles(), syst->box);
//
//	return 0;
//}
