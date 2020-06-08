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

	if(files.size() == 0) {
		std::string error = boost::str(boost::format("Pattern '%s' did not return any filename, cannot initialise a trajectory from an empty file list!") % tot_path.string());
		throw std::runtime_error(error);
	}

	initialise_from_filelist(files);
}

std::shared_ptr<System> BaseTrajectory::_filtered_system(std::shared_ptr<System> system) {
	for(auto filter : _filters) {
		system = filter->filter(system);
	}

	return system;
}

#ifdef PYTHON_BINDINGS

void export_BaseTrajectory(py::module &m) {
	py::class_<BaseTrajectory, PyBaseTrajectory, std::shared_ptr<BaseTrajectory>> parser(m, "BaseTrajectory");

	parser.def(py::init<std::shared_ptr<BaseParser>>());

	parser.def("initialise_from_folder", &BaseTrajectory::initialise_from_folder);

	parser.def("initialise_from_filelist", &BaseTrajectory::initialise_from_filelist);

	parser.def("initialise_from_trajectory_file", &BaseTrajectory::initialise_from_trajectory_file);

	parser.def("next_frame", &BaseTrajectory::next_frame, R"pbdoc(
        Return the next frame (system), or :code:`None` if we reached the end of the trajectory.

        Returns
        -------
            :class:`System`
                Either the next system or :code:`None` if there are no more available frames.
	)pbdoc");

	parser.def("reset", &BaseTrajectory::reset, R"pbdoc(
        Reset the trajectory so that a call to :meth:`next_frame` will return the first frame of the trajectory.
	)pbdoc");

	parser.def("add_filter", &BaseTrajectory::add_filter, py::arg("filter"), R"pbdoc(
		Add a filter to the list of filters that will be applied to each frame (system) that compose the trajectory. 

		The order according to which filters are added to the trajectory is important, since they are applied 
		to systems in a "first-in first-out" fashion.

		Parameters
		----------
		filter : :class:BaseFilter
			The new filter.
	)pbdoc");
}

#endif

} /* namespace ba */
