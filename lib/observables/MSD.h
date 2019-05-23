/*
 * MSD.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_MSD_H_
#define OBSERVABLES_MSD_H_

#include "../trajectories/BaseTrajectory.h"

#include <map>

namespace ba {

class MSD {
public:
	// TODO: pass the trajectory directly to the compute and compute_and_print methods
	MSD(std::shared_ptr<BaseTrajectory> trajectory);
	virtual ~MSD();

	std::map<ullint, double> compute(uint points_per_cycle, bool remove_com);
	void compute_and_print(uint points_per_cycle, bool remove_com, std::string output_file);

protected:
	double _conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second, bool remove_com);

protected:
	std::shared_ptr<BaseTrajectory> _trajectory;
};

#ifdef PYTHON_BINDINGS

void export_MSD(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_MSD_H_ */
