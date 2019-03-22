/*
 * BaseTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "BaseTrajectory.h"

namespace ba {

using namespace std;

BaseTrajectory::BaseTrajectory(shared_ptr<BaseParser> parser) :
				_parser(parser) {

}

BaseTrajectory::~BaseTrajectory() {

}

void BaseTrajectory::add_filter(shared_ptr<BaseFilter> filter) {
	_filters.push_back(filter);
}

#ifdef PYTHON_BINDINGS

void export_BaseTrajectory(py::module &m) {
	pybind11::class_<BaseTrajectory, PyBaseTrajectory, std::shared_ptr<BaseTrajectory>> parser(m, "BaseTrajectory");

	parser
		.def(py::init<shared_ptr<BaseParser>>())
		.def("initialise_from_folder", &BaseTrajectory::initialise_from_folder)
		.def("initialise_from_trajectory_file", &BaseTrajectory::initialise_from_trajectory_file)
		.def("next_frame", &BaseTrajectory::next_frame)
		.def("reset", &BaseTrajectory::reset)
		.def("add_filter", &BaseTrajectory::add_filter);
}

#endif

} /* namespace ba */
