/*
 * MSD.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "MSD.h"

#include <fstream>

using namespace std;

namespace ba {

MSD::MSD(uint points_per_cycle, bool remove_com) :
				TrajectoryObservable<std::map<ullint, double>>(),
				_points_per_cycle(points_per_cycle),
				_remove_com(remove_com) {
	if(points_per_cycle < 1) {
		string error = fmt::format("The MSD's points per cycle ({}) should larger than 0", points_per_cycle);
		throw std::runtime_error(error);
	}
}

MSD::~MSD() {

}

void MSD::analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) {
	map<ullint, uint> n_conf;

	shared_ptr<System> current_cycle_base;
	vector<shared_ptr<System>> past_cycle_bases;

	uint N_first_conf = 0;
	uint idx = 0;

	auto frame = trajectory->next_frame();
	while(frame != nullptr) {
		uint N_conf = frame->N();
		if(N_first_conf == 0) {
			N_first_conf = N_conf;
			BA_INFO("Computing the MSD on configurations composed of {} particles", N_conf);
		}

		if(N_conf != N_first_conf) {
			string error = fmt::format("The configurations at times 0 and {} contain different number of particles (%d != {})", frame->time, N_first_conf, N_conf);
			throw std::runtime_error(error);
		}

		if(idx % _points_per_cycle == 0) {
			if(idx > 0) {
				// save the old cycle base
				past_cycle_bases.push_back(current_cycle_base);
			}

			current_cycle_base = frame;

			// loop over configurations that were the bases of past cycles
			for(auto past_base : past_cycle_bases) {
				double cc_MSD = _conf_conf_MSD(current_cycle_base, past_base, _remove_com);
				ullint time_diff = current_cycle_base->time - past_base->time;
				auto it = _result.find(time_diff);
				if(it == _result.end()) {
					_result[time_diff] = cc_MSD;
					n_conf[time_diff] = 1;
				}
				else {
					_result[time_diff] += cc_MSD;
					n_conf[time_diff]++;
				}
			}
		}

		// don't compute the MSD of a configuration with itself
		if(current_cycle_base != frame) {
			double cc_MSD = _conf_conf_MSD(current_cycle_base, frame, _remove_com);
			ullint time_diff = frame->time - current_cycle_base->time;
			auto it = _result.find(time_diff);
			if(it == _result.end()) {
				_result[time_diff] = cc_MSD;
				n_conf[time_diff] = 1;
			}
			else {
				_result[time_diff] += cc_MSD;
				n_conf[time_diff]++;
			}
		}

		idx++;
		frame = trajectory->next_frame();
	}

	for(auto &pair : _result) {
		if(pair.first > 0) {
			pair.second /= n_conf[pair.first];
		}
	}
}

void MSD::analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file) {
	analyse_trajectory(trajectory);

	ofstream output(output_file);

	for(auto pair : _result) {
		if(pair.first > 0) {
			output << pair.first << " " << pair.second << endl;
		}
	}

	output.close();
}

double MSD::_conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second, bool remove_com) {
	double cc_MSD = 0.;
	uint N = first->N();
	vec3 com_diff(0., 0., 0.);
	if(remove_com) {
		com_diff = second->com() - first->com();
	}
	for(uint i = 0; i < N; i++) {
		vec3 diff = second->particles()[i]->position() - first->particles()[i]->position() - com_diff;
		cc_MSD += glm::dot(diff, diff);
	}

	return cc_MSD / N;
}

#ifdef PYTHON_BINDINGS

void export_MSD(py::module &m) {
	py::class_<MSD, std::shared_ptr<MSD>> obs(m, "MSD", "Compute the `mean-squared displacement <https://en.wikipedia.org/wiki/Mean_squared_displacement>`_ of a system.");

	obs.def(py::init<uint, bool>());
	obs.def("analyse_and_print", &MSD::analyse_and_print, py::arg("trajectory"), py::arg("output_file"), R"pb(
Analyse the trajectory and print the MSD directly to the given file.
	)pb");

	PY_EXPORT_TRAJECTORY_OBS(obs, MSD);
}

#endif

} /* namespace ba */
