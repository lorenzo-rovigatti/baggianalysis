/*
 * LAMMPSDumpParser.h
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_LAMMPSDUMPPARSER_H_
#define PARSERS_LAMMPSDUMPPARSER_H_

#include "BaseParser.h"

namespace ba {

class LAMMPSDumpParser: public BaseParser {
public:
	LAMMPSDumpParser(std::string data_file, std::string atom_style);
	LAMMPSDumpParser();
	LAMMPSDumpParser(std::shared_ptr<System> data_file_system);
	virtual ~LAMMPSDumpParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

private:
	struct CoordField {
		int idx = Fields::UNSET;
		bool scaled = false;
		bool wrapped = true;
	};

	struct Fields {
		static const int UNSET = -1;

		int idx = UNSET;
		int type= UNSET;
		int charge = UNSET;
		int mass = UNSET;
		std::array<int, 3> vel = { UNSET, UNSET, UNSET };
		std::array<CoordField, 3> coords;
	};

	int _parse_headers(std::ifstream &configuration, std::shared_ptr<System> syst);

	Fields _atom_fields;
	std::shared_ptr<System> _data_file_system = nullptr;
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDumpParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_LAMMPSDUMPPARSER_H_ */
