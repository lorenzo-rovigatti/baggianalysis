/*
 * MolecularCOMs.cpp
 *
 *  Created on: 1 apr 2021
 *      Author: lorenzo
 */

#include "MolecularCOMs.h"

namespace ba {

MolecularCOMs::MolecularCOMs() {

}

MolecularCOMs::~MolecularCOMs() {

}

std::shared_ptr<System> MolecularCOMs::filter(std::shared_ptr<const System> syst) {
	std::shared_ptr<System> new_syst(syst->empty_copy());

	for(auto molecule : syst->molecules()) {
		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(new_syst->available_index(), DEFAULT_PARTICLE_TYPE, molecule->com(), molecule->velocity()));
		new_particle->set_mass(molecule->mass());
		new_particle->set_charge(molecule->charge());
		new_syst->add_particle(new_particle);
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_MolecularCOMs(py::module &m) {
	py::class_<MolecularCOMs, BaseFilter, std::shared_ptr<MolecularCOMs>> filter(m, "MolecularCOMs", R"pbdoc(
A filter that takes each molecule of the given system and adds it as a particle in the new system using the molecule's centre of mass, velocity, charge and mass.
)pbdoc");

	filter.def(py::init<>(), R"pbdoc(
The constructor takes no parameters.
	)pbdoc");
}

#endif

} /* namespace ba */
