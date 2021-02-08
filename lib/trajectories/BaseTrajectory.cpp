/*
 * BaseTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "BaseTrajectory.h"

#include "../utils/filesystem.h"

namespace ba {

BaseTrajectory::BaseTrajectory(std::shared_ptr<BaseParser> parser) :
				_parser(parser) {

}

BaseTrajectory::~BaseTrajectory() {

}

void BaseTrajectory::add_filter(std::shared_ptr<BaseFilter> filter) {
	_filters.push_back(filter);
}

void BaseTrajectory::initialise_from_folder(std::string folder, std::string pattern, bool natural_sorting) {
	if(!utils::exists(folder)) {
		std::string error = fmt::format("The '{}' folder does not exist", folder);
		throw std::runtime_error(error);
	}

	if(!utils::is_directory(folder)) {
		std::string error = fmt::format("'{}' is not a folder", folder);
		throw std::runtime_error(error);
	}

	std::string tot_path = utils::join_paths(folder, pattern);
	std::vector<std::string> files = utils::glob(tot_path, natural_sorting);

	if(files.size() == 0) {
		std::string error = fmt::format("Pattern '{}' did not return any filename, cannot initialise a trajectory from an empty file list!", tot_path);
		throw std::runtime_error(error);
	}

	initialise_from_filelist(files);
}

void BaseTrajectory::set_include_system(SystemIncluder func) {
	_include_system = func;
}

std::shared_ptr<System> BaseTrajectory::_filtered_system(std::shared_ptr<System> system) {
	for(auto filter : _filters) {
		system = filter->filter(system);
	}

	return system;
}

#ifdef PYTHON_BINDINGS

void export_BaseTrajectory(py::module &m) {
	py::class_<BaseTrajectory, PyBaseTrajectory, std::shared_ptr<BaseTrajectory>> parser(m, "BaseTrajectory", R"pbdoc(
        An interface that defines the method that can be used to initialise and manage trajectories.

        Child classes should at least implement :meth:`initialise_from_filelist`, :meth:`initialise_from_trajectory_file`,
        :meth:`next_frame` and :meth:`reset`. 
    )pbdoc");

	parser.def(py::init<std::shared_ptr<BaseParser>>(), "The constructor takes a parser as its only parameter.");

	parser.def("initialise_from_folder", &BaseTrajectory::initialise_from_folder, py::arg("folder"), py::arg("pattern"), py::arg("natural_sorting") = true, R"pbdoc(
        Initialise the trajectory by loading up frames stored in files contained in a folder according to a pattern.

        Parameters
        ----------
        folder : str
            The folder containing the files to be parsed
        pattern : str
            A glob pattern used to select the files to be loaded. Examples are *confs\** or *conf?0000.dat*.
        natural_sorting : bool
            Sort the files alphabetically while, at the same time, attempting to handle multi-digit numbers.   
	)pbdoc");

	parser.def("initialise_from_filelist", &BaseTrajectory::initialise_from_filelist, py::arg("filelist"), R"pbdoc(
        Initialise the trajectory out of a list of filenames.

        Parameters
        ----------
        filelist : List(str)
            The list of filenames that will be used to initialise the trajectory.
	)pbdoc");

	parser.def("initialise_from_trajectory_file", &BaseTrajectory::initialise_from_trajectory_file, py::arg("trajectory_file"), R"pbdoc(
        Initialise the trajectory from a single file, which should contain all the frames, one after the other.

        Parameters
        ----------
        trajectory_file : str
            The name of the file containing the trajectory.
	)pbdoc");

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
		filter : :class:`BaseFilter`
			The new filter.
	)pbdoc");

	parser.def("set_include_system", &BaseTrajectory::set_include_system, py::arg("system_includer"), R"pbdoc(
        Set an optional function that will be used to test whether a system should be included in the trajectory or not. 
        The argument should be a callable that takes a :class:`System` as its only parameter and returns True 
        if the system should be included, False otherwise.

        Here is an example that will include only those systems having timesteps larger than 1e4::

            def include_system(system):
                return system.time > 1e4

            trajectory.set_include_system(include_system)

        The same result can be obtained with a lambda function::

            trajectory.set_include_system(lambda syst: syst.time > 1e4)

        Parameters
        ----------
        system_includer : callable
            A callable that takes the :class:`System` to be checked a returns a boolean that specifies whether the system should
            be included in the trajectory or not.
	)pbdoc");
}

#endif

} /* namespace ba */
