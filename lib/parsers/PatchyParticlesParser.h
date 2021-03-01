/*
 * PatchyParticlesParser.h
 *
 *  Created on: 18 feb 2021
 *      Author: lorenzo
 */

#ifndef PARSERS_PATCHYPARTICLESPARSER_H_
#define PARSERS_PATCHYPARTICLESPARSER_H_

#include "BaseParser.h"

namespace ba {

class PatchyParticlesParser: public BaseParser {
public:
	PatchyParticlesParser();
	PatchyParticlesParser(std::string patch_filename);
	PatchyParticlesParser(std::vector<vec3> base_patches);
	virtual ~PatchyParticlesParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

protected:
	std::vector<vec3> _base_patches;

	void _set_base_patches(std::vector<vec3> base_patches);
};

#ifdef PYTHON_BINDINGS

void export_PatchyParticlesParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_PATCHYPARTICLESPARSER_H_ */
