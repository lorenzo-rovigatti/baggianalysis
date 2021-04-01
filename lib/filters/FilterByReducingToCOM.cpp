/*
 * FilterByReducingToCOM.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByReducingToCOM.h"

using namespace std;

namespace ba {

FilterByReducingToCOM::FilterByReducingToCOM() {

}

FilterByReducingToCOM::~FilterByReducingToCOM() {

}

shared_ptr<System> FilterByReducingToCOM::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	// the new system will contain a single particle, so we give it index 0
	shared_ptr<Particle> new_particle(std::make_shared<Particle>(0, DEFAULT_PARTICLE_TYPE, syst->com(), syst->velocity()));
	new_syst->add_particle(new_particle);

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByReducingToCOM(py::module &m) {
	py::class_<FilterByReducingToCOM, BaseFilter, std::shared_ptr<FilterByReducingToCOM>> filter(m, "FilterByReducingToCOM", R"pbdoc(
A filter that builds a new system containing a single particle whose position and velocity are the position and velocity of the 
centre of mass of the input system.
	)pbdoc");

	filter.def(py::init<>(), R"pbdoc(
    The constructor does not take any parameters.
	)pbdoc");
}

#endif

} /* namespace ba */
