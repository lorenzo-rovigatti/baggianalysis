/*
 * test_feature.cpp
 *
 *  Created on: Sep 4, 2019
 *      Author: lorenzo
 */


#include <parsers/GenericOxDNAParser.h>
#include <trajectories/LazyTrajectory.h>
#include <neighbour_finders/SANNFinder.h>

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " topology configuration" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::BaseParser> parser(new ba::GenericOxDNAParser(argv[1]));
	std::shared_ptr<ba::LazyTrajectory> trajectory(new ba::LazyTrajectory(parser));
	trajectory->initialise_from_trajectory_file(argv[2]);

	std::shared_ptr<ba::SANNFinder> finder(new ba::SANNFinder(2.8, ba::SANNFinder::SYMMETRISE_BY_REMOVING));

	auto syst = trajectory->next_frame();
	finder->set_neighbours(syst->particles(), syst->box);

	return 0;
}
