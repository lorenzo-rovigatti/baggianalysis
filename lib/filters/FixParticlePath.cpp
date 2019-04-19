/*
 * FixParticlePath.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "FixParticlePath.h"

namespace ba {

using namespace std;

FixParticlePath::FixParticlePath() {

}

FixParticlePath::~FixParticlePath() {

}

shared_ptr<System> FixParticlePath::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	if(_previous_frame != nullptr) {
		if(syst->time < _previous_frame->time) {
			string error = boost::str(boost::format("The current frame's timestep (%llu) is smaller than the previous frame's timestep (%llu)") % syst->time % _previous_frame->time);
			throw runtime_error(error);
		}
		if(syst->box != _previous_frame->box) {
			string error = boost::str(boost::format("Fixing the particle path is available only when the box size does not change during the course of the trajectory (current time: %llu, previous time: %llu)") % syst->time % _previous_frame->time);
			throw runtime_error(error);
		}

		for(uint i = 0; i < syst->N(); i++) {
			auto p = syst->particles()[i];

			shared_ptr<Particle> new_particle(new Particle(*p.get()));
			vec3 shift = vec3(_shifts[i]) * syst->box;
			vec3 new_pos = p->position() + shift;

			vec3 diff = new_pos - _previous_frame->particles()[i]->position();
			for(uint d = 0; d < 3; d++) {
				double half_box = 0.5 * syst->box[d];
				if(diff[d] > half_box) {
					new_pos[d] -= syst->box[d];
					_shifts[i][d]--;
				}
				else if(diff[d] < -half_box) {
					new_pos[d] += syst->box[d];
					_shifts[i][d]++;
				}
			}
			new_particle->set_position(new_pos);
			new_syst->add_particle(new_particle);
		}
	}
	else {
		_shifts.resize(syst->N(), particle_shift(0, 0, 0));
		for(auto p : syst->particles()) {
			shared_ptr<Particle> new_particle(new Particle(*p.get()));
			new_syst->add_particle(p);
		}
	}

	_previous_frame = new_syst;

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FixParticlePath(py::module &m) {
	pybind11::class_<FixParticlePath, BaseFilter, std::shared_ptr<FixParticlePath>> filter(m, "FixParticlePath");

	filter
		.def(py::init<>())
		.def("filter", &FixParticlePath::filter);
}

#endif

} /* namespace ba */
