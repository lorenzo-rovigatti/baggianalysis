/*
 * SystemObservable.h
 *
 *  Created on: 19 dic 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_INTERFACES_SYSTEMOBSERVABLE_H_
#define OBSERVABLES_INTERFACES_SYSTEMOBSERVABLE_H_

#include "TrajectoryObservable.h"

namespace ba {

template<typename result_type>
class SystemObservable: public TrajectoryObservable<result_type> {
public:
	SystemObservable() :
					TrajectoryObservable<result_type>() {

	}

	virtual ~SystemObservable() {

	}

	void analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) override {
		TrajectoryObservable<result_type>::reset();
		auto frame = trajectory->next_frame();
		while(frame != nullptr) {
			analyse_system(frame);
			frame = trajectory->next_frame();
		}
		trajectory->reset();
	}

	virtual void analyse_system(std::shared_ptr<System> system) = 0;
};

#ifdef PYTHON_BINDINGS

#define PY_EXPORT_SYSTEM_OBS(instance_name, class_name) {\
															instance_name\
																.def("analyse_trajectory", &class_name::analyse_trajectory)\
																.def("analyse_system", &class_name::analyse_system)\
																.def("reset", &class_name::reset)\
																.def("result", &class_name::result);\
														}\

#endif

} /* namespace ba */

#endif /* OBSERVABLES_INTERFACES_SYSTEMOBSERVABLE_H_ */
