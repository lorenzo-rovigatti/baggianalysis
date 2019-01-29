/*
 * main.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include <Trajectory.h>

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " topology trajectory" << std::endl;
		return 1;
	}

	auto trajectory = ba::Trajectory();
	trajectory.setTopologyFile(argv[1]);
	trajectory.setTrajectoryFile(argv[2]);
	trajectory.initialise();

	return 0;
}
