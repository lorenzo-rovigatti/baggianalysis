/*
 * CumulativePoreSize.h
 *
 *  Created on: May 23, 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_PORESIZE_H_
#define OBSERVABLES_PORESIZE_H_

#include "../trajectories/BaseTrajectory.h"

namespace ba {

class PoreSize {
public:
	/**
	 * @brief Construct the object
	 *
	 * @param N_attempts the number of randomly chosen points on which the pore size is to be computed
	 */
	PoreSize(int N_attempts);
	virtual ~PoreSize();

	/**
	 * @brief Set the minimum size of the cells used to compute the pore size
	 *
	 * This method impacts only the code's performances.
	 */
	void set_r_cut(double r_cut);

	/**
	 * @brief Set the size of the particles
	 *
	 * Since all particles are supposed to be spherical, this size is meant to be the distance between a particle's centre and its surface and therefore controls the pore size.
	 *
	 * @param particle_radius the radius that will be used to compute the pore size
	 */
	void set_particle_radius(double particle_radius);

	/**
	 * @brief Calculates the radius of the largest sphere centred in "centre" that does not overlap with any of the particles
	 */
	double radius(const vec3 &centre);

	/**
	 *	@brief Compute the pore size of the given trajectory, sampled at randomly chosen points.
	 *
	 * @param trajectory the trajectory to be analysed
	 * @return a vector containing all the computed pore sizes. The size of the vector is equal or less than the number of attempts times the number of frames in the trajectory
	 */
	vector_scalar compute(std::shared_ptr<BaseTrajectory> trajectory);

public:
	// TODO: make the radius_wrapper and constraint functions friend to this class and make these two class members private
	std::shared_ptr<System> current_frame = nullptr;
	vec3 current_position;

private:
	int _N_attempts;
	double _r_cut;
	double _particle_radius;
	double _particle_radius_sqr;
	glm::ivec3 _N_cells_side;
	std::vector<int> _next;
	std::vector<int> _heads;
	std::vector<std::vector<glm::ivec3>> _cell_shifts;

	void _init_cells();
	int _get_cell_index(const vec3 &pos) const;
	glm::ivec3 _get_cell(const vec3 &pos) const;
};

#ifdef PYTHON_BINDINGS

void export_PoreSize(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_PORESIZE_H_ */
