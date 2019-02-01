/*
 * Trajectory.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORIES_FULLTRAJECTORY_H_
#define TRAJECTORIES_FULLTRAJECTORY_H_

#include "BaseTrajectory.h"

namespace ba {

class FullTrajectory : public BaseTrajectory {
public:
	FullTrajectory(std::shared_ptr<BaseParser> parser);
	virtual ~FullTrajectory();

	void add_filter(std::shared_ptr<BaseFilter> filter) override;
	void initialise_from_folder(std::string folder, std::string pattern) override;
	void initialise_from_trajectory_file(std::string trajectory_file) override;

	virtual std::shared_ptr<System> next_frame() override;

public:
	std::vector<std::shared_ptr<System>> frames;

protected:
	std::vector<std::shared_ptr<System>>::iterator _current_system;
};

} /* namespace ba */

#endif /* TRAJECTORIES_FULLTRAJECTORY_H_ */
