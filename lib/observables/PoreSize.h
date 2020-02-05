/*
 * CumulativePoreSize.h
 *
 *  Created on: May 23, 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_PORESIZE_H_
#define OBSERVABLES_PORESIZE_H_

#include "interfaces/SystemObservable.h"

#include "../utils/CellLists.h"

#include <nlopt.hpp>

namespace ba {

class PoreSize: public SystemObservable<vector_scalar> {
public:
	/**
	 * @brief Construct the object
	 *
	 * @param N_attempts the number of randomly chosen points on which the pore size is to be computed
	 * @param r_cut the minimum size of the cells used to compute the pore size. It impacts the code's performances and nothing else
	 * @param particle_radius the distance between a particle's centre and its surface
	 * @param maxtime the timeout (in seconds) for the optimisation procedure
	 */
	PoreSize(int N_attempts, double r_cut = 1.0, double particle_radius = 0.5, double max_time = 1.0);
	PoreSize() = delete;
	virtual ~PoreSize();

	/**
	 * @brief Calculates the radius of the largest sphere centred in "centre" that does not overlap with any of the particles
	 */
	double radius(const vec3 &centre);

	/**
	 *	@brief Compute the pore size of the given system, sampled at randomly chosen points.
	 *
	 * @param frame the system to be analysed
	 * @return a vector containing all the computed pore sizes. The size of the vector is equal or less than the number of attempts
	 */
	void analyse_system(std::shared_ptr<System> frame) override;

public:
	// TODO: make the radius_wrapper and constraint functions friend to this class and make these two class members private
	std::shared_ptr<System> current_frame = nullptr;
	vec3 current_position;

private:
	int _N_attempts;
	double _r_cut;
	double _particle_radius;
	double _particle_radius_sqr;
	double _maxtime;

	CellLists _lists;

	nlopt::opt _local_opt;
	nlopt::opt _opt;
};

#ifdef PYTHON_BINDINGS

void export_PoreSize(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_PORESIZE_H_ */
