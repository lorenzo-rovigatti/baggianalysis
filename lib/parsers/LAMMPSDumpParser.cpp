/*
 * LAMMPSDumpParser.cpp
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#include "LAMMPSDumpParser.h"

#include "LAMMPSDataFileParser.h"
#include "../utils/strings.h"

namespace ba {

LAMMPSDumpParser::LAMMPSDumpParser(std::string data_file, std::string atom_style) :
				BaseParser() {
	LAMMPSDataFileParser parser = LAMMPSDataFileParser(atom_style);
	_data_file_system = parser.make_system(data_file);
	set_topology(Topology::make_topology_from_system(_data_file_system.get()));
}

LAMMPSDumpParser::LAMMPSDumpParser(std::shared_ptr<System> data_file_system) :
				BaseParser(),
				_data_file_system(data_file_system) {
	set_topology(std::make_shared<Topology>(data_file_system.get()));
}

LAMMPSDumpParser::LAMMPSDumpParser() :
				BaseParser() {

}

LAMMPSDumpParser::~LAMMPSDumpParser() {

}

std::shared_ptr<System> LAMMPSDumpParser::_parse_stream(std::ifstream &configuration) {
	std::shared_ptr<System> syst(std::make_shared<System>());

	int N = _parse_headers(configuration, syst);

	if(N == -1) {
		return nullptr;
	}

	for(int i = 0; i < N; i++) {
		std::string line;
		std::getline(configuration, line);
		if(!configuration.good()) {
			std::string error = fmt::format("The line relative to the {}-th particle cannot be read", i);
			throw std::runtime_error(error);
		}

		std::string to_split = utils::trim_copy(line);
		auto split = utils::split(to_split);

		if(split.size() < 5) {
			std::string error = fmt::format("The LAMMPS dump file should contain at least 5 columns ({} found)", split.size());
			throw std::runtime_error(error);
		}

		try {
			int p_idx = (_atom_fields.idx == Fields::UNSET) ? syst->available_index() : utils::lexical_cast<int>(split[_atom_fields.idx]);
			std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(p_idx));

			if(_atom_fields.type != Fields::UNSET) {
				new_particle->set_type(split[_atom_fields.type]);
			}

			if(_atom_fields.mass != Fields::UNSET) {
				new_particle->set_mass(utils::lexical_cast<double>(split[_atom_fields.mass]));
			}

			if(_atom_fields.charge != Fields::UNSET) {
				new_particle->set_charge(utils::lexical_cast<double>(split[_atom_fields.charge]));
			}

			vec3 pos, vel;
			for(int i = 0; i < 3; i++) {
				// i-th coordinate
				if(_atom_fields.coords[i].idx != Fields::UNSET) {
					pos[i] = utils::lexical_cast<double>(split[_atom_fields.coords[i].idx]);
					if(_atom_fields.coords[i].scaled) {
						pos[i] *= syst->box[i];
					}
				}
				// i-th velocity
				if(_atom_fields.vel[i] != Fields::UNSET) {
					vel[i] = utils::lexical_cast<double>(split[_atom_fields.vel[i]]);
				}
			}
			new_particle->set_position(pos);
			new_particle->set_velocity(vel);

			syst->add_particle(new_particle);
		}
		catch(utils::bad_lexical_cast &e) {
			std::string error = fmt::format("The line relative to the {}-th particle cannot be parsed because one or more fields cannot be cast to numeric data types", i);
			throw std::runtime_error(error);
		}

	}

	// copy masses and charges from the system's data file if the dump file does not contain info about them
	if(_data_file_system != nullptr) {
		for(auto p : syst->particles()) {
			auto other_p = _data_file_system->particle_by_id(p->index());
			if(_atom_fields.mass == Fields::UNSET) {
				p->set_mass(other_p->mass());
			}
			if(_atom_fields.charge == Fields::UNSET) {
				p->set_charge(other_p->charge());
			}
		}
	}

	return syst;
}

int LAMMPSDumpParser::_parse_headers(std::ifstream &configuration, std::shared_ptr<System> syst) {
	int N = -1;

	bool done = false;
	while(!done) {
		std::string line;
		std::getline(configuration, line);
		if(!configuration.good()) {
			return -1;
		}
		if(utils::starts_with(line, "ITEM:")) {
			if(utils::contains(line, "TIMESTEP")) {
				std::getline(configuration, line);
				syst->time = utils::lexical_cast<ullint>(utils::trim_copy(line));
			}
			else if(utils::contains(line, "NUMBER OF ATOMS")) {
				std::getline(configuration, line);
				try {
					N = utils::lexical_cast<uint>(utils::trim_copy(line));
				}
				catch(utils::bad_lexical_cast &e) {
					std::string error = fmt::format("The number of particles '{}' found in the LAMMPS dump configuration cannot be cast to an integer", line);
					throw std::runtime_error(error);
				}
			}
			else if(utils::contains(line, "BOX BOUNDS")) {
				// the next three lines contains the box dimensions along the three axes
				for(uint i = 0; i < 3; i++) {
					std::getline(configuration, line);
					std::string to_split = utils::trim_copy(line);
					auto split = utils::split(to_split);
					try {
						double lower = utils::lexical_cast<double>(split[0]);
						double upper = utils::lexical_cast<double>(split[1]);
						syst->box[i] = upper - lower;
					}
					catch(utils::bad_lexical_cast &e) {
						std::string error = fmt::format("The box line '{}' found in the LAMMPS dump configuration is not valid", line);
						throw std::runtime_error(error);
					}
				}
			}
			else if(utils::contains(line, "ITEM: ATOMS")) {
				auto spl = utils::split(line);
				std::vector<std::string> unused_fields;
				for(uint i = 2; i < spl.size(); i++) {
					int idx = i - 2;
					std::string attribute = utils::trim_copy(spl[i]);
					if(attribute.size() > 0) {
						if(attribute == "id") {
							_atom_fields.idx = idx;
						}
						else if(attribute == "type") {
							_atom_fields.type = idx;
						}
						else if(attribute == "mass") {
							_atom_fields.mass = idx;
						}
						else if(attribute == "q") {
							_atom_fields.charge = idx;
						}
						else if(attribute == "vx") {
							_atom_fields.vel[0] = idx;
						}
						else if(attribute == "vy") {
							_atom_fields.vel[1] = idx;
						}
						else if(attribute == "vz") {
							_atom_fields.vel[2] = idx;
						}
						// unscaled, wrapped coordinates
						else if(attribute == "x") {
							_atom_fields.coords[0].idx = idx;
						}
						else if(attribute == "y") {
							_atom_fields.coords[1].idx = idx;
						}
						else if(attribute == "z") {
							_atom_fields.coords[2].idx = idx;
						}
						// scaled, wrapped coordinates
						else if(attribute == "xs") {
							_atom_fields.coords[0].idx = idx;
							_atom_fields.coords[0].scaled = true;
						}
						else if(attribute == "ys") {
							_atom_fields.coords[1].idx = idx;
							_atom_fields.coords[1].scaled = true;
						}
						else if(attribute == "zs") {
							_atom_fields.coords[2].idx = idx;
							_atom_fields.coords[2].scaled = true;
						}
						// unscaled, unwrapped coordinates
						else if(attribute == "xu") {
							_atom_fields.coords[0].idx = idx;
							_atom_fields.coords[0].wrapped = false;
						}
						else if(attribute == "yu") {
							_atom_fields.coords[1].idx = idx;
							_atom_fields.coords[1].wrapped = false;
						}
						else if(attribute == "zu") {
							_atom_fields.coords[2].idx = idx;
							_atom_fields.coords[2].wrapped = false;
						}
						// scaled, unwrapped coordinates
						else if(attribute == "xsu") {
							_atom_fields.coords[0].idx = idx;
							_atom_fields.coords[0].scaled = true;
							_atom_fields.coords[0].wrapped = false;
						}
						else if(attribute == "ysu") {
							_atom_fields.coords[1].idx = idx;
							_atom_fields.coords[1].scaled = true;
							_atom_fields.coords[1].wrapped = false;
						}
						else if(attribute == "zsu") {
							_atom_fields.coords[2].idx = idx;
							_atom_fields.coords[2].scaled = true;
							_atom_fields.coords[2].wrapped = false;
						}
						else {
							unused_fields.push_back(attribute);
						}
					}
				}

				if(unused_fields.size() > 0) {
					std::string warning = "The -->";
					for(auto &f: unused_fields) {
						warning += " " + f;
					}
					warning += " <-- field(s) found in the dump file relative to time = {} will not be used";
					BA_WARNING(warning, syst->time);
				}

				done = true;
			}
		}
	}

	return N;
}

#ifdef PYTHON_BINDINGS

void export_LAMMPSDumpParser(py::module &m) {
	py::class_<LAMMPSDumpParser, BaseParser, std::shared_ptr<LAMMPSDumpParser>> parser(m, "LAMMPSDumpParser", R"pbdoc(
Parse LAMMPS dump files.

It uses the "ITEM: ATOMS" line to automatically map fields to atom/particle properties (type, index, mass, charge, coordinates, velocities).
It can optionally use a LAMMPS data file to also initialise the system topology.
)pbdoc");

	parser.def(py::init<>(), "The default constructor takes no arguments");
	parser.def(py::init<std::shared_ptr<System>>(), py::arg("data_file_system"));
	parser.def(py::init<std::string, std::string>(), py::arg("data_file"), py::arg("atom_style"));
}

#endif

} /* namespace ba */
