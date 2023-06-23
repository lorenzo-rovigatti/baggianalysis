/*
 * CollectiveISF.h
 *
 *  Created on: Jun 21, 2023
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_COLLECTIVEISF_H_
#define OBSERVABLES_COLLECTIVEISF_H_

#include "interfaces/TrajectoryObservable.h"
#include "../utils/WaveVectorList.h"

namespace ba {

class CollectiveISF: public TrajectoryObservable<std::map<ullint, std::vector<double>>> {
public:
	CollectiveISF(double largest_q, uint max_n_realisations, double max_delta_q, uint points_per_cycle);
	CollectiveISF(const WaveVectorList &q_vectors, uint points_per_cycle);
	CollectiveISF() = delete;
	virtual ~CollectiveISF();

	void analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) override;
	void analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file, bool normalise);

protected:
	void _add_value(ullint time_diff, std::vector<double> values, std::map<ullint, uint> &n_conf);
	std::vector<double> _conf_conf_CollectiveISF(std::shared_ptr<System> first, std::shared_ptr<System> second);

	int _points_per_cycle;

	WaveVectorList _q_list;
};

#ifdef PYTHON_BINDINGS

void export_CollectiveISF(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_COLLECTIVEISF_H_ */
