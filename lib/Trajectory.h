/*
 * Trajectory.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "System.h"
#include "filters/BaseFilter.h"

namespace ba {

class Trajectory {
public:
	Trajectory();
	virtual ~Trajectory();

	void setTopologyFile(std::string topology_file);
	void setTrajectoryFile(std::string trajectory_file);

	void add_filter(std::shared_ptr<BaseFilter> filter);
	void initialise();

public:
	std::vector<std::shared_ptr<System>> frames;

protected:
	std::string _topology_file;
	std::string _trajectory_file;

	std::vector<std::shared_ptr<BaseFilter>> _filters;
};

} /* namespace ba */

#endif /* TRAJECTORY_H_ */
