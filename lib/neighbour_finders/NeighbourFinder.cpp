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

#ifdef PYTHON_BINDINGS

void export_NeighbourFinder(py::module &m) {
	py::class_<NeighbourFinder, PyNeighbourFinder, std::shared_ptr<NeighbourFinder>> finder(m, "NeighbourFinder");

	finder.def(py::init<>());
	finder.def("set_neighbours", &NeighbourFinder::set_neighbours);
}

#endif

} /* namespace ba */
