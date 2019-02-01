/*
 * BaseTrajectory.h
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORIES_BASETRAJECTORY_H_
#define TRAJECTORIES_BASETRAJECTORY_H_

#include "../filters/BaseFilter.h"
#include "../parsers/BaseParser.h"
#include "../System.h"

namespace ba {

class BaseTrajectory {
public:
	BaseTrajectory(std::shared_ptr<BaseParser> parser);
	virtual ~BaseTrajectory();

	virtual void add_filter(std::shared_ptr<BaseFilter> filter);
	virtual void initialise_from_folder(std::string folder, std::string pattern) = 0;
	virtual void initialise_from_trajectory_file(std::string trajectory_file) = 0;

	virtual std::shared_ptr<System> next_frame() = 0;

protected:
	std::shared_ptr<BaseParser> _parser;
	std::vector<std::shared_ptr<BaseFilter>> _filters;
};

} /* namespace ba */

#endif /* TRAJECTORIES_BASETRAJECTORY_H_ */
