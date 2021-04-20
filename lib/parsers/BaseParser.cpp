/*
 * BaseParser.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "BaseParser.h"

#include "../topology/Topology.h"

namespace ba {

BaseParser::BaseParser() {

}

BaseParser::~BaseParser() {

}

void BaseParser::use_topology(bool use) {
	_use_topology = use;
}

void BaseParser::set_topology(std::shared_ptr<Topology> new_topology) {
	_topology = new_topology;
}

void BaseParser::_apply_topology(std::shared_ptr<System> system) {
	if(_topology != nullptr && _use_topology && system != nullptr) {
		_topology->apply(system);
	}
}

std::shared_ptr<System> BaseParser::make_system(std::ifstream &configuration) {
	auto new_system = _parse_stream(configuration);
	_apply_topology(new_system);

	return new_system;
}

std::shared_ptr<System> BaseParser::make_system(std::string conf_name) {
	auto new_system = _parse_file(conf_name);
	_apply_topology(new_system);

	return new_system;
}

std::shared_ptr<System> BaseParser::_parse_file(std::string conf_name) {
	std::ifstream conf_file(conf_name);

	if(!conf_file.good()) {
		std::string error = fmt::format("Unreadable configuration file '{}'", conf_name);
		throw std::runtime_error(error);
	}

	auto new_system = _parse_stream(conf_file);
	conf_file.close();

	return new_system;
}

std::shared_ptr<System> BaseParser::_parse_stream(std::ifstream &configuration) {
	std::string error = fmt::format("Unimplemented 'BaseParser::_parse_stream' method invoked. The error may have been caused by trying to use a custom Python parser to initialise a trajectory through the 'initialise_from_trajectory_file()', which is forbidden");
	throw std::runtime_error(error);

	return nullptr;
}

#ifdef PYTHON_BINDINGS

void export_BaseParser(py::module &m) {
	py::class_<BaseParser, PyBaseParser, std::shared_ptr<BaseParser>> parser(m, "BaseParser");

	parser.def(py::init<>());
	parser.def("set_topology", &BaseParser::set_topology);
	parser.def("use_topology", &BaseParser::use_topology);
	// we export to python the version that takes the filename only
	parser.def("make_system", (std::shared_ptr<System> (BaseParser::*)(std::string)) &BaseParser::make_system);
	parser.def("_parse_file", (std::shared_ptr<System> (BaseParser::*)(std::string)) &BaseParser::_parse_file);
}

#endif

} /* namespace ba */
