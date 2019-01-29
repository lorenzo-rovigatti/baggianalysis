/*
 * main.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include <Trajectory.h>

#include <observables/MSD.h>

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " topology trajectory" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::Trajectory> trajectory(new ba::Trajectory());
	trajectory->setTopologyFile(argv[1]);
	trajectory->setTrajectoryFile(argv[2]);
	trajectory->initialise();

	ba::MSD msd(trajectory);

	auto opts = ba::MSD::default_options();
	opts.points_per_cycle = 20;
	msd.compute_and_print(opts);

	return 0;
}
