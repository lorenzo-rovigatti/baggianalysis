/*
 * FilterByType.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByType.h"

#include <algorithm>

using namespace std;

namespace ba {

FilterByType::FilterByType(std::vector<particle_type> allowed_types) :
				BaseFilter(),
				_allowed_types(allowed_types) {

}

FilterByType::~FilterByType() {

}

shared_ptr<System> FilterByType::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	for(auto p : syst->particles()) {
		particle_type p_type = p->type();
		if(find(_allowed_types.begin(), _allowed_types.end(), p_type) != _allowed_types.end()) {
			shared_ptr<Particle> new_particle(new Particle(*p.get()));
			new_syst->add_particle(new_particle);
		}
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByType(py::module &m) {
	pybind11::class_<FilterByType, BaseFilter, std::shared_ptr<FilterByType>> filter(m, "FilterByType");

	filter
		.def(py::init<std::vector<particle_type>>())
		.def("filter", &FilterByType::filter);
}

#endif

} /* namespace ashell */
