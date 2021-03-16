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
	LAMMPSDumpParser(std::string data_file, std::string atom_style, bool rescaled_coords=false);
	LAMMPSDumpParser(bool rescaled_coords=false);
	LAMMPSDumpParser(std::shared_ptr<System> data_file_system, bool rescaled_coords=false);
	virtual ~LAMMPSDumpParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

private:
	struct HeaderData {
		uint N = 0;
		vec3 box = vec3(0., 0., 0.);
		ullint time_step = 0;
		bool empty = false;
	};

	HeaderData _parse_headers(std::ifstream &configuration, std::shared_ptr<System> syst);

	bool _rescaled_coords;
	std::shared_ptr<System> _data_file_system = nullptr;
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDumpParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_LAMMPSDUMPPARSER_H_ */
