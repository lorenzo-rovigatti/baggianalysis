/*
 * BaseTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "BaseTrajectory.h"

#include "../utils/filesystem.h"

#include <boost/filesystem.hpp>

namespace ba {

namespace bfs = boost::filesystem;

BaseTrajectory::BaseTrajectory(std::shared_ptr<BaseParser> parser) :
				_parser(parser) {

}

BaseTrajectory::~BaseTrajectory() {

}

void BaseTrajectory::add_filter(std::shared_ptr<BaseFilter> filter) {
	_filters.push_back(filter);
}

void BaseTrajectory::initialise_from_folder(std::string folder, std::string pattern, bool natural_sorting) {
	bfs::path path(folder);

	if(!bfs::exists(path)) {
		std::string error = boost::str(boost::format("The '%s' folder does not exist") % folder);
		throw std::runtime_error(error);
	}

	if(!bfs::is_directory(path)) {
		std::string error = boost::str(boost::format("'%s' is not a folder") % folder);
		throw std::runtime_error(error);
	}

	bfs::path tot_path = bfs::path(folder) / bfs::path(pattern);
	std::vector<std::string> files = utils::glob(tot_path.string(), natural_sorting);

	initialise_from_filelist(files);
}

#ifdef PYTHON_BINDINGS

void export_BaseTrajectory(py::module &m) {
	py::class_<BaseTrajectory, PyBaseTrajectory, std::shared_ptr<BaseTrajectory>> parser(m, "BaseTrajectory");

	parser
		.def(py::init<std::shared_ptr<BaseParser>>())
		.def("initialise_from_folder", &BaseTrajectory::initialise_from_folder)
		.def("initialise_from_filelist", &BaseTrajectory::initialise_from_filelist)
		.def("initialise_from_trajectory_file", &BaseTrajectory::initialise_from_trajectory_file)
		.def("next_frame", &BaseTrajectory::next_frame)
		.def("reset", &BaseTrajectory::reset)
		.def("add_filter", &BaseTrajectory::add_filter);
}

#endif

} /* namespace ba */
