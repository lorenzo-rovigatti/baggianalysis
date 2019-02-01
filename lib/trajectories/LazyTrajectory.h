/*
 * LazyTrajectory.h
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORIES_LAZYTRAJECTORY_H_
#define TRAJECTORIES_LAZYTRAJECTORY_H_

#include "BaseTrajectory.h"

#include <fstream>

namespace ba {

class LazyTrajectory: public BaseTrajectory {
public:
	LazyTrajectory(std::shared_ptr<BaseParser> parser);
	virtual ~LazyTrajectory();

	void initialise_from_folder(std::string folder, std::string pattern) override;
	void initialise_from_trajectory_file(std::string trajectory_file) override;

	virtual std::shared_ptr<System> next_frame() override;
	virtual void reset() override;

protected:
	enum {
		UNINITIALISED,
		FOLDER,
		TRAJECTORY_FILE
	};

	int _type;
	std::string _path;
	std::ifstream _trajectory_file;
	std::vector<std::string> _files;
	std::vector<std::string>::iterator _current_file;
};

} /* namespace ba */

#endif /* TRAJECTORIES_LAZYTRAJECTORY_H_ */
