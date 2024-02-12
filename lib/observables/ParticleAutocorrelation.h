/*
 * ParticleAutocorrelation.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_PARTICLEAUTOCORRELATION_H_
#define OBSERVABLES_PARTICLEAUTOCORRELATION_H_

#include "interfaces/TrajectoryObservable.h"

#include <map>

namespace ba {

using AccessorType = std::function<vec3(Particle *)>;

class ParticleAutocorrelation: public TrajectoryObservable<std::map<ullint, double>> {
public:
	ParticleAutocorrelation(uint points_per_cycle, AccessorType, bool normalise);
	ParticleAutocorrelation() = delete;
	virtual ~ParticleAutocorrelation();

	void analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) override;
	void analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file);

protected:
	void _add_value(ullint time_diff, double value, std::map<ullint, uint> &n_conf);
	double _conf_conf_value(std::shared_ptr<System> first, std::shared_ptr<System> second);

	uint _points_per_cycle;
	AccessorType _accessor_function;
	bool _normalise;
};

#ifdef PYTHON_BINDINGS

void export_ParticleAutocorrelation(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_PARTICLEAUTOCORRELATION_H_ */
