/*
 * SelfISF.h
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_SELFISF_H_
#define OBSERVABLES_SELFISF_H_

#include "interfaces/TrajectoryObservable.h"

namespace ba {

class SelfISF: public TrajectoryObservable<std::map<ullint, std::vector<double>>> {
public:
	SelfISF(double largest_q, uint max_n_realisations, double max_delta_q, uint points_per_cycle);
	virtual ~SelfISF();

	void analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) override;

protected:
	void _init_qs(std::shared_ptr<System> system);
	void _add_value(ullint time_diff, std::vector<double> values, std::map<ullint, uint> &n_conf);
	std::vector<double> _conf_conf_SelfISF(std::shared_ptr<System> first, std::shared_ptr<System> second);

	int _points_per_cycle;
	double _largest_q;
	uint _max_n_realisations;
	double _max_delta_q;

	std::map<double, std::vector<vec3>> _q_vectors;
	vec3 _last_box = vec3(0., 0., 0.);
};

#ifdef PYTHON_BINDINGS

void export_SelfISF(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_SELFISF_H_ */
