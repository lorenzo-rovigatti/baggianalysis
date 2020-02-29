/*
 * LAMMPSDataFileParser.h
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_LAMMPSDATAFILEPARSER_H_
#define PARSERS_LAMMPSDATAFILEPARSER_H_

#include "BaseParser.h"

namespace ba {
/**
 * Parses a LAMMPS data file. See here for info about the file format: https://lammps.sandia.gov/doc/read_data.html
 */
class LAMMPSDataFileParser: public BaseParser {
public:
	LAMMPSDataFileParser(std::string atom_style);
	virtual ~LAMMPSDataFileParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

private:
	struct HeaderData {
		uint N_atoms = 0;
		uint N_bonds = 0;
		uint atom_types = 0;
		uint bond_types = 0;
		vec3 box = vec3(1., 1., 1.);
		bool empty = false;
	};

	HeaderData _parse_headers(std::ifstream &configuration);
	double _parse_box_line(std::vector<std::string> split_line);
	std::string _read_line(std::ifstream &configuration);

	std::string _atom_style;
	std::vector<std::string> _LAMMPS_section_keywords;

	int _type_index;
	int _pos_starting_index;
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_LAMMPSDATAFILEPARSER_H_ */
