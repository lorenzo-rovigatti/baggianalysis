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
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " data_file folder" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::LAMMPSDataFileParser> data_parser = std::make_shared<ba::LAMMPSDataFileParser>("full");
	auto data_system = data_parser->make_system(argv[1]);


//	std::shared_ptr<ba::LAMMPSDumpParser> parser = std::make_shared<ba::LAMMPSDumpParser>(data_system);
//	std::shared_ptr<ba::FullTrajectory> trajectory = std::make_shared<ba::FullTrajectory>(parser);
//	trajectory->initialise_from_folder(argv[2], "pvdf_0", true);

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
