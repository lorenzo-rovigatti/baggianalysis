/*
 * NoTopology.cpp
 *
 *  Created on: 25 sep 2021
 *      Author: lorenzo
 */

#include "NoTopology.h"

#include <fstream>

namespace ba {
namespace oxDNA_topology {

NoTopology::NoTopology() :
				Default() {
}

NoTopology::~NoTopology() {

}

particle_type NoTopology::type(int p_idx) {
	return "0";
}

#ifdef PYTHON_BINDINGS

void export_NoTopology(py::module &m) {
	py::class_<NoTopology, std::shared_ptr<NoTopology>, Default> parser(m, "NoTopology");

	parser.def(py::init<>());
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */
