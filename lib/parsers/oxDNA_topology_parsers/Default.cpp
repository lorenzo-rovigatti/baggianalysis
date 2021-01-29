/*
 * Default.cpp
 *
 *  Created on: 11 gen 2021
 *      Author: lorenzo
 */

#include "Default.h"

#include <fstream>

namespace ba {
namespace oxDNA_topology {

Default::Default(std::string topology_file) {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = fmt::format("Topology file '{}' not found", topology_file);
		throw std::runtime_error(error);
	}

	topology >> _N;
	topology >> _N_A;
	if(!topology.good()) {
		_N_A = 0;
	}

	topology.close();
}

Default::~Default() {

}

uint Default::N() {
	return _N;
}

particle_type Default::type(int p_idx) {
	if(_N_A == 0 || (uint) p_idx < _N_A) {
		return "0";
	}
	return "1";
}

#ifdef PYTHON_BINDINGS

void export_Default(py::module &m) {
	py::class_<Default, PyDefault, std::shared_ptr<Default>> parser(m, "Default");

	parser.def(py::init<std::string>());
	parser.def("N", &Default::N);
	parser.def("type", &Default::type);
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */
