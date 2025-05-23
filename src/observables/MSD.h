/*
 * MSD.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_MSD_H_
#define OBSERVABLES_MSD_H_

#include "interfaces/TrajectoryObservable.h"

#include <map>

namespace ba {

class MSD: public TrajectoryObservable<std::map<ullint, double>> {
public:
	MSD(uint points_per_cycle, bool remove_com);
	MSD() = delete;
	virtual ~MSD();

	void analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) override;
	void analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file);

protected:
	void _add_value(ullint time_diff, double cc_MSD, std::map<ullint, uint> &n_conf);
	double _conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second, bool remove_com);

	uint _points_per_cycle;
	bool _remove_com;
};

void export_MSD(py::module &m);

} /* namespace ba */

#endif /* OBSERVABLES_MSD_H_ */
