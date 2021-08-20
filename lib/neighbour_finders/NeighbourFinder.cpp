/*
 * NNFinder.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "NeighbourFinder.h"

namespace ba {

NeighbourFinder::NeighbourFinder() {

}

NeighbourFinder::~NeighbourFinder() {

}

void NeighbourFinder::include_bonded_neighbours(bool include) {
	_include_bonded_neighbours = include;
}

void NeighbourFinder::_add_neighbour(std::shared_ptr<Particle> add_to, std::shared_ptr<Particle> to_be_added) {
	if(_include_bonded_neighbours || !add_to->has_bonded_neighbour(to_be_added)) {
		add_to->add_neighbour(to_be_added);
	}
}

void NeighbourFinder::_add_neighbour(std::shared_ptr<Particle> add_to, std::string type, std::shared_ptr<Particle> to_be_added) {
	if(_include_bonded_neighbours || !add_to->has_bonded_neighbour(to_be_added)) {
		add_to->add_neighbour(type, to_be_added);
	}
}

#ifdef PYTHON_BINDINGS

void export_NeighbourFinder(py::module &m) {
	py::class_<NeighbourFinder, PyNeighbourFinder, std::shared_ptr<NeighbourFinder>> finder(m, "NeighbourFinder",
			"Interface for classes that set the neighbours of a given list of particles according to some criterium.");

	finder.def(py::init<>());

	finder.def("set_neighbours", &NeighbourFinder::set_neighbours, py::arg("particles"), py::arg("box"), R"pbdoc(
Set the neighbours of the given list of particles. Note that only particles that belong to the given list can be neighbours.
The criterium according to which particles are defined as neighbours can be set by subclassing this interface and overriding 
this method.

By default if particle ``q`` is already a bonded neighbour of particle ``p``, ``q`` will not be added as a regular neighbour of ``p``.
This behaviour can be changed by invoking :meth:`include_bonded_neighbours`.

Parameters
----------
    particles : List(:class:`Particle`)
        The particles whose neighbours will be set.
    box : numpy.ndarray
        The simulation box.
)pbdoc");

	finder.def("include_bonded_neighbours", &NeighbourFinder::include_bonded_neighbours, py::arg("include"), R"pbdoc(

Parameters
----------
    include : bool
        True if bonded neighbours can also be regular neighbours, False otherwise.
)pbdoc");
}

#endif

} /* namespace ba */
