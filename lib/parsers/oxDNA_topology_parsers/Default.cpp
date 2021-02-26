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

uint Default::N_A() {
	return _N_A;
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
	parser.def("N", &Default::N, R"pbdoc(
        Return the number of particles as specified in the topology file.

        Returns
        -------
        int
            The number of particles as specified in the topology file.

    )pbdoc");

	parser.def("N_A", &Default::N_A, R"pbdoc(
        A default oxDNA topology contains either a single number (the total number of particles, *N*) or two numbers 
        (*N* and the number of particles of the first species, *NA*). This method returns *N* in the former case and
        *NA* in the latter.   

        Returns
        -------
        int
            The number of particles of species *A* as specified in the topology file.

	)pbdoc");

	parser.def("type", &Default::type, py::arg("p_idx"), R"pbdoc(
        The type of the particle with the given index.

        Parameters
        ----------
        p_idx: int
            The index of the particle whose type one wants to know.

        Returns
        -------
        str
            The type of the given particle.

	)pbdoc");
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */
