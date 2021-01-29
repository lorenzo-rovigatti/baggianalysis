/*
 * TSP.cpp
 *
 *  Created on: 11 gen 2021
 *      Author: lorenzo
 */

#include "TSP.h"

namespace ba {
namespace oxDNA_topology {

TSP::TSP(std::string topology_file) {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = fmt::format("Topology file '{}' not found", topology_file);
		throw std::runtime_error(error);
	}

	topology >> _N_stars;
	_N_monomers_per_arm.resize(_N_stars);
	_N_arms.resize(_N_stars);

	_N = 1;
	for(uint i = 0; i < _N_stars; i++) {
		uint N_per_a, N_a;
		double alpha;
		topology >> N_a >> N_per_a >> alpha;

		_N += N_per_a * N_a;
		_N_monomers_per_arm[i] = N_per_a;
		_N_arms[i] = N_a;
	}

	topology.close();
}

TSP::~TSP() {

}

uint TSP::N_stars() const {
	return _N_stars;
}

uint TSP::N_monomers_per_arm(uint star) const {
	return _N_monomers_per_arm[star];
}

uint TSP::N_arms(int star) const {
	return _N_arms[star];
}

#ifdef PYTHON_BINDINGS

void export_TSP(py::module &m) {
	py::class_<TSP, Default, std::shared_ptr<TSP>> parser(m, "TSP");

	parser.def(py::init<std::string>());
	parser.def("N_stars", &TSP::N_stars);
	parser.def("N_monomers_per_arm", &TSP::N_monomers_per_arm);
	parser.def("N_arms", &TSP::N_arms);
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */
