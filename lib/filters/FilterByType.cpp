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
			shared_ptr<Particle> new_particle(std::make_shared<Particle>(*p.get()));
			new_syst->add_particle(new_particle);
		}
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByType(py::module &m) {
	py::class_<FilterByType, BaseFilter, std::shared_ptr<FilterByType>> filter(m, "FilterByType", R"pbdoc(
A filter that builds a new system by including only those particles whose type is in the list provided by the user.

As an example, the following snippet creates a filter that will include in the new system only particles of type
"0" and "3"::

    allowed_types = ["0", "3"]
    my_filter = ba.FilterByType(allowed_types)

	)pbdoc");

	filter.def(py::init<std::vector<particle_type>>(), R"pbdoc(
        The constructor takes a list of particle types as its only parameter.

        Parameters
        ----------
        allowed_types : List(str)
            The list of particle types that should be included in the new system.
	)pbdoc");
}

#endif

} /* namespace ashell */
