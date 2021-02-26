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
	LAMMPSDataFileParser(int type_index, int pos_starting_index);
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

	const std::vector<std::string> _LAMMPS_section_keywords = std::vector<std::string>( {
		"Atoms", "Velocities", "Masses", "Ellipsoids", "Lines", "Triangles", "Bodies",
		"Bonds", "Angles", "Dihedrals", "Impropers",
		"Pair Coeffs", "PairIJ Coeffs", "Bond Coeffs", "Angle Coeffs", "Dihedral Coeffs", "Improper Coeffs",
		"BondBond Coeffs", "BondAngle Coeffs", "MiddleBondTorsion Coeffs", "EndBondTorsion Coeffs",
		"AngleTorsion Coeffs", "AngleAngleTorsion Coeffs", "BondBond13 Coeffs", "AngleAngle Coeffs"
	});
	const std::vector<std::string> _header_entries = {
		"atoms", "bonds", "angles", "dihedrals", "impropers", "atom types", "bond types", "angle types",
		"dihedral types", "improper types", "extra bond per atom", "extra angle per atom", "extra dihedral per atom",
		"extra improper per atom", "extra special per atom", "ellipsoids", "lines", "triangles", "bodies",
		"xlo xhi", "ylo yhi", "zlo zhi", "xy xz yz"
	};

	int _type_index;
	int _pos_starting_index;
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_LAMMPSDATAFILEPARSER_H_ */
