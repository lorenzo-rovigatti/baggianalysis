/*
 * BaseTrajectory.h
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#ifndef TRAJECTORIES_BASETRAJECTORY_H_
#define TRAJECTORIES_BASETRAJECTORY_H_

#include "../filters/BaseFilter.h"
#include "../parsers/BaseParser.h"
#include "../System.h"

namespace ba {

using SystemIncluder = std::function<bool(std::shared_ptr<System>)>;

class BaseTrajectory {
public:
	/**
	 * @brief Construct the object
	 *
	 * @param parser the parser that should be used to construct frames
	 */
	BaseTrajectory(std::shared_ptr<BaseParser> parser);
	virtual ~BaseTrajectory();

	/**
	 * @brief Add a filter to the list of filters used to modify each frame
	 *
	 * The order according to which filters are added to the trajectory is important, since they are applied to the single frames in a "first-in first-out" fashion.
	 *
	 * @param filter the new filter
	 */
	virtual void add_filter(std::shared_ptr<BaseFilter> filter);

	/**
	 * @brief Initialise the trajectory from the files contained in the given folder and whose names match the given pattern
	 *
	 * The list of files that will be parsed is generated with C's glob function (see "man glob" for additional details).
	 *
	 * @param folder the name of the folder
	 * @param pattern the glob-compatible pattern used to select the files of interest
	 * @param natural_sorting whether the files found in the folder should be sorted or not
	 */
	virtual void initialise_from_folder(std::string folder, std::string pattern, bool natural_sorting);

	/**
	 * @brief Initialise the trajectory using the given list of files
	 *
	 * @param filelist the list of files to be parsed
	 */
	virtual void initialise_from_filelist(std::vector<std::string> filelist) = 0;

	/**
	 * @brief Initialise the trajectory from a single file containing one or more frames
	 *
	 * @param trajectory_file the file to be parsed
	 */
	virtual void initialise_from_trajectory_file(std::string trajectory_file) = 0;

	void set_include_system(SystemIncluder func);

	virtual std::shared_ptr<System> next_frame() = 0;
	virtual void reset() = 0;

protected:
	std::shared_ptr<BaseParser> _parser;
	std::vector<std::shared_ptr<BaseFilter>> _filters;

	SystemIncluder _include_system = [](std::shared_ptr<System>) {
		return true;
	};

	virtual std::shared_ptr<System> _filtered_system(std::shared_ptr<System> system);
};

#ifdef PYTHON_BINDINGS

class PyBaseTrajectory : public BaseTrajectory {
public:
	using BaseTrajectory::BaseTrajectory;

	void initialise_from_filelist(std::vector<std::string> filelist) override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				void,
				BaseTrajectory,
				initialise_from_filelist,
				filelist
		);
	}

	void initialise_from_trajectory_file(std::string trajectory_file) override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				void,
				BaseTrajectory,
				initialise_from_trajectory_file,
				trajectory_file
		);
	}

	std::shared_ptr<System> next_frame() override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				std::shared_ptr<System>,
				BaseTrajectory,
				next_frame,
		);

		// suppress warnings
		return std::shared_ptr<System>(std::make_shared<System>());
	}

	void reset() override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				void,
				BaseTrajectory,
				reset,
		);
	}
};

void export_BaseTrajectory(py::module &m);

#endif

} /* namespace ba */

#endif /* TRAJECTORIES_BASETRAJECTORY_H_ */
