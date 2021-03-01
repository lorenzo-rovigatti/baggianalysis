/*
 * PatchyParticlesParser.cpp
 *
 *  Created on: 18 gen 2021
 *      Author: lorenzo
 */

#include "PatchyParticlesParser.h"

#include "../utils/strings.h"

#include <glm/gtx/orthonormalize.hpp>

namespace ba {

PatchyParticlesParser::PatchyParticlesParser() :
				BaseParser() {
	std::vector<vec3> base_patches;

	double half_isqrt3 = 0.5 / sqrt(3);
	base_patches.emplace_back(vec3(-half_isqrt3, -half_isqrt3,  half_isqrt3));
	base_patches.emplace_back(vec3( half_isqrt3, -half_isqrt3, -half_isqrt3));
	base_patches.emplace_back(vec3( half_isqrt3,  half_isqrt3,  half_isqrt3));
	base_patches.emplace_back(vec3(-half_isqrt3,  half_isqrt3, -half_isqrt3));

	_set_base_patches(base_patches);
}

PatchyParticlesParser::PatchyParticlesParser(std::string patch_filename) {
	std::ifstream patch_file(patch_filename);
	if(!patch_file.good()) {
		std::string error = fmt::format("Unreadable patch file '{}' ", patch_filename);
		throw std::runtime_error(error);
	}

	std::string line;
	std::getline(patch_file, line);

	int N_patches;
	try {
		N_patches = utils::lexical_cast<int>(utils::trim_copy(line));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("Invalid number of patches '{}' found in the patch file", line);
		throw std::runtime_error(error);
	}

	std::vector<vec3> base_patches;
	for(int i = 0; i < N_patches; i++) {
		std::getline(patch_file, line);
		base_patches.emplace_back(utils::vector_from_line(line));
	}
	patch_file.close();

	_set_base_patches(base_patches);
}

PatchyParticlesParser::PatchyParticlesParser(std::vector<vec3> base_patches) :
				BaseParser() {
	_set_base_patches(base_patches);
}

PatchyParticlesParser::~PatchyParticlesParser() {

}

void PatchyParticlesParser::_set_base_patches(std::vector<vec3> base_patches) {
	for(auto &patch : base_patches) {
		patch = glm::normalize(patch);
	}

	_base_patches = base_patches;
}

std::shared_ptr<System> PatchyParticlesParser::_parse_stream(std::ifstream &configuration) {
	std::string line;
	uint N;

	// the header line
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(std::make_shared<System>());

	auto split = utils::split(line);
	try {
		syst->time = utils::lexical_cast<ullint>(utils::trim_copy(split[0]));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The timestep value '{}' found in the configuration cannot be cast to an integer", split[2]);
		throw std::runtime_error(error);
	}

	try {
		N = utils::lexical_cast<uint>(utils::trim_copy(split[1]));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The parsed number of particles '{}' found in the configuration cannot be cast to an integer", split[2]);
		throw std::runtime_error(error);
	}

	try {
		syst->box[0] = utils::lexical_cast<double>(utils::trim_copy(split[2]));
		syst->box[1] = utils::lexical_cast<double>(utils::trim_copy(split[3]));
		syst->box[2] = utils::lexical_cast<double>(utils::trim_copy(split[4]));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The box line '{}' found in the configuration is not valid", line);
		throw std::runtime_error(error);
	}

	for(uint i = 0; i < N; i++) {
		std::getline(configuration, line);
		vec3 p1 = utils::vector_from_line(line);

		std::getline(configuration, line);
		vec3 p2 = utils::vector_from_line(line);

		std::getline(configuration, line);
		vec3 position = utils::vector_from_line(line);
		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(syst->available_index(), DEFAULT_PARTICLE_TYPE, position, vec3(0., 0., 0.)));

		// build the orientation matrix
		glm::normalize(p1);
		glm::normalize(p2);
		vec3 p3 = glm::cross(p1, p2);

		glm::dmat3 orientation_matrix(p1, p2, p3);
		orientation_matrix = glm::transpose(orientation_matrix);
		glm::orthonormalize(orientation_matrix);

		for(auto &patch : _base_patches) {
			new_particle->add_orientation_vector(orientation_matrix * patch);
		}

		syst->add_particle(new_particle);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_PatchyParticlesParser(py::module &m) {
	py::class_<PatchyParticlesParser, BaseParser, std::shared_ptr<PatchyParticlesParser>> parser(m, "PatchyParticlesParser", R"pbdoc(
        Build systems out of `PatchyParticles <https://github.com/lorenzo-rovigatti/PatchyParticles>`_ configurations. These files
        store the particles' positios and orientation matrices.
    )pbdoc");

	parser.def(py::init<>(), R"pbdoc(
Default constructor: the parser will assume that the particles have four patches arranged on a tetrahedron.
    )pbdoc");

	parser.def(py::init<std::vector<vec3>>(), py::arg("base_patches"),  R"pbdoc(
The object can also be built by providing a list of vectors storing the *base patches*, that is, those vectors that, when
multiplied by the orientation matrix, yield the particles' patches.
	)pbdoc");

	parser.def(py::init<std::string>(), py::arg("patch_filename"), R"pbdoc(
Obtain the list of vectors storing the *base patches* from the given file.
	)pbdoc");
}

#endif

} /* namespace ba */
