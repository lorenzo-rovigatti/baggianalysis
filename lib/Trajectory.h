/*
 * Trajectory.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "System.h"

namespace ba {

class Trajectory {
public:
	Trajectory();
	virtual ~Trajectory();

	void setTopologyFile(std::string topology_file);
	void setTrajectoryFile(std::string trajectory_file);

	void initialise();

public:
	std::vector<std::shared_ptr<System>> frames;

protected:
	std::string _topology_file;
	std::string _trajectory_file;

};

} /* namespace ba */

#endif /* TRAJECTORY_H_ */
