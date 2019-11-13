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

	virtual void add_filter(std::shared_ptr<BaseFilter> filter) override;

	/**
	 * @brief Initialise the trajectory using the given list of files.
	 *
	 * This method sorts the frames according to their timesteps after they have been all parsed.
	 *
	 * @param filelist the list of files to be parsed
	 */
	virtual void initialise_from_filelist(std::vector<std::string> filelist) override;

	virtual void initialise_from_trajectory_file(std::string trajectory_file) override;

	virtual std::shared_ptr<System> next_frame() override;
	virtual void reset() override;

	std::vector<std::shared_ptr<System>> frames;

protected:
	std::vector<std::shared_ptr<System>>::iterator _current_system;
};

#ifdef PYTHON_BINDINGS

void export_FullTrajectory(py::module &m);

#endif

} /* namespace ba */

#endif /* TRAJECTORIES_FULLTRAJECTORY_H_ */
