/*
 * TopologyParsers.h
 *
 *  Created on: 14 nov 2019
 *      Author: lorenzo
 */

#ifndef TOPOLOGY_TOPOLOGYPARSERS_H_
#define TOPOLOGY_TOPOLOGYPARSERS_H_

#include "Topology.h"

namespace ba {

void parse_microgel_bondfile(std::string, std::shared_ptr<Topology>);
void parse_polymer_bondfile(std::string filename, std::shared_ptr<Topology> topology);
std::shared_ptr<Topology> topology_from_LAMMPS_data_file(std::string filename);
void parse_LAMMPS_topology(std::string, std::shared_ptr<Topology>);

#ifdef PYTHON_BINDINGS

void export_TopologyParsers(py::module &m);

#endif

} /* namespace ba */

#endif /* TOPOLOGY_TOPOLOGYPARSERS_H_ */
