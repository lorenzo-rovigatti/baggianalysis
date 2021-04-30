/*
 * TrajectoryObservable.h
 *
 *  Created on: 19 dic 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_INTERFACES_TRAJECTORYOBSERVABLE_H_
#define OBSERVABLES_INTERFACES_TRAJECTORYOBSERVABLE_H_

#include "../../trajectories/BaseTrajectory.h"

namespace ba {

template<typename result_type>
class TrajectoryObservable {
public:
	TrajectoryObservable() {

	}

	virtual ~TrajectoryObservable() {

	}

	virtual void reset() {
		_result = result_type{};
	}

	virtual void analyse_trajectory(std::shared_ptr<BaseTrajectory>) = 0;

	virtual result_type result() {
		result_type res = _finalised_result();
		return res;
	}

protected:
	virtual result_type _finalised_result() {
		return _result;
	}

	result_type _result;
};

#ifdef PYTHON_BINDINGS

#define PY_EXPORT_TRAJECTORY_OBS(instance_name, class_name) {\
																instance_name\
																	.def("analyse_trajectory", &class_name::analyse_trajectory)\
																	.def("reset", &class_name::reset)\
																	.def("result", &class_name::result);\
															}\

#endif

} /* namespace ba */

#endif /* OBSERVABLES_INTERFACES_TRAJECTORYOBSERVABLE_H_ */
