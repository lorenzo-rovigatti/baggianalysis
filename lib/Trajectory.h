/*
 * Trajectory.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "filters/BaseFilter.h"
#include "parsers/BaseParser.h"
#include "System.h"

namespace ba {

class Trajectory {
public:
	Trajectory(std::shared_ptr<BaseParser> parser);
	virtual ~Trajectory();

	void add_filter(std::shared_ptr<BaseFilter> filter);
	void initialise_from_folder(std::string folder, std::string pattern);
	void initialise_from_trajectory_file(std::string trajectory_file);

public:
	std::vector<std::shared_ptr<System>> frames;

protected:
	std::shared_ptr<BaseParser> _parser;
	std::vector<std::shared_ptr<BaseFilter>> _filters;
};

} /* namespace ba */

#endif /* TRAJECTORY_H_ */
