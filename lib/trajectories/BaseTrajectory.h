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

class BaseTrajectory {
public:
	BaseTrajectory(std::shared_ptr<BaseParser> parser);
	virtual ~BaseTrajectory();

	virtual void add_filter(std::shared_ptr<BaseFilter> filter);
	virtual void initialise_from_folder(std::string folder, std::string pattern) = 0;
	virtual void initialise_from_trajectory_file(std::string trajectory_file) = 0;

	virtual std::shared_ptr<System> next_frame() = 0;
	virtual void reset() = 0;

protected:
	std::shared_ptr<BaseParser> _parser;
	std::vector<std::shared_ptr<BaseFilter>> _filters;
};

#ifdef PYTHON_BINDINGS

class PyBaseTrajectory : public BaseTrajectory {
public:
	using BaseTrajectory::BaseTrajectory;

	void initialise_from_folder(std::string folder, std::string pattern) override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				void,
				BaseTrajectory,
				initialise_from_folder,
				folder,
				pattern
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
		return std::shared_ptr<System>(new System());
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
