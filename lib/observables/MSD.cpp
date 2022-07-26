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

void MSD::_add_value(ullint time_diff, double cc_MSD, std::map<ullint, uint> &n_conf) {
	// in some cases rounding issues will make time differences differ by 1 when instead they should coincide.
	// The following predicate looks for time differences that are equal or smaller than 1
	auto pred = [time_diff](const auto &other_diff) {
		return std::abs((long long int) time_diff - (long long int) other_diff.first) <= 1;
	};

	// we apply the above predicate to the map
	auto it = std::find_if(_result.begin(), _result.end(), pred);
	if(it == _result.end()) {
		_result[time_diff] = cc_MSD;
		n_conf[time_diff] = 1;
	}
	// if a compatible time difference (i.e. the same or one that differs by 1) is found then we use that to update the data structures
	else {
		_result[it->first] += cc_MSD;
		n_conf[it->first]++;
	}
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

				_add_value(time_diff, cc_MSD, n_conf);
			}
		}

		// don't compute the MSD of a configuration with itself
		if(current_cycle_base != frame) {
			double cc_MSD = _conf_conf_MSD(current_cycle_base, frame, _remove_com);
			ullint time_diff = frame->time - current_cycle_base->time;

			_add_value(time_diff, cc_MSD, n_conf);
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
	py::class_<MSD, std::shared_ptr<MSD>> obs(m, "MSD", R"pb(
Compute the `mean-squared displacement <https://en.wikipedia.org/wiki/Mean_squared_displacement>`_ of a trajectory.

The trajectory is split up in chunks of size `points_per_cycle`, and the mean squared displacement is computed between configurations in each chunk and
between the initial configurations of pairs of chunks.

The mean squared displacement between two configurations is associated to their time delta (*i.e.* to the difference between the times at
which they have been printed) and averaged with all the other pairs that share the same time delta. Note that in same cases (*i.e.* log or log-linear 
spacing), there might be time deltas that differ by one: in this case the code consider those pairs to share the same time delta.   
)pb");

	obs.def(py::init<uint, bool>(), py::arg("points_per_cycle"), py::arg("remove_com") = true, R"pb(
Parameters
----------
points_per_cycle: int
    The number of configurations contained in each chunk in which the trajectory is split up.
remove_com: bool
    Remove the centre of mass' position from each configuration.
)pb");

	obs.def("analyse_and_print", &MSD::analyse_and_print, py::arg("trajectory"), py::arg("output_file"), R"pb(
Analyse the trajectory and print the MSD directly to the given file.
	)pb");

	PY_EXPORT_TRAJECTORY_OBS(obs, MSD);
}

#endif

} /* namespace ba */
