/*
 * DNA.cpp
 *
 *  Created on: 29 sep 2023
 *      Author: lorenzo
 */

#include "DNA.h"

namespace ba {
namespace oxDNA_topology {

DNA::DNA(std::string topology_file) :
		Default() {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = fmt::format("Topology file '{}' not found", topology_file);
		throw std::runtime_error(error);
	}

	int N_strands;

	topology >> _N;
	topology >> N_strands;

	_types.resize(_N, "A");
	_bonded_neighbours.resize(_N);

	for(uint i = 0; i < _N; i++) {
		int N_strand, n3, n5;
		topology >> N_strand >> _types[i] >> n3 >> n5;

		if(!topology.good()) {
			std::string error = fmt::format("The topology should contain one line per nucleotide, but it seems there is info for only {} nucleotides\n", i);
			throw std::runtime_error(error);
		}

		if(n3 != -1) {
			_bonded_neighbours[i].push_back(n3);
		}

		if(n5 != -1) {
			_bonded_neighbours[i].push_back(n5);
		}
	}

	topology.close();
}

DNA::~DNA() {

}

particle_type DNA::type(int idx) {
	return _types[idx];
}

#ifdef PYTHON_BINDINGS

void export_DNA(py::module &m) {
	py::class_<DNA, std::shared_ptr<DNA>, Default> parser(m, "DNA");

	parser.def(py::init<std::string>());
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */