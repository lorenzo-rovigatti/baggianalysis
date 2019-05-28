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
	PoreSize(int N_attempts);
	virtual ~PoreSize();

	void set_r_cut(double r_cut);
	void set_particle_radius(double particle_radius);
	void set_maxtime(double maxtime);

	/// calculates the radius of the largest sphere centred in "centre" that does not overlap with any of the particles
	double radius(const vec3 &centre);

	vector_scalar compute(std::shared_ptr<BaseTrajectory> trajectory);

public:
	std::shared_ptr<System> current_frame = nullptr;
	vec3 current_position;

protected:
	int _N_attempts;
	double _r_cut;
	double _particle_radius;
	double _particle_radius_sqr;
	double _maxtime;
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
