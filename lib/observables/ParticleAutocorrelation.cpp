/*
 * ParticleAutocorrelation.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "ParticleAutocorrelation.h"

#include <fstream>

using namespace std;

namespace ba {

ParticleAutocorrelation::ParticleAutocorrelation(uint points_per_cycle, AccessorType func, bool normalise) :
				TrajectoryObservable<std::map<ullint, double>>(),
				_points_per_cycle(points_per_cycle),
				_accessor_function(func),
				_normalise(normalise) {
	if(points_per_cycle < 1) {
		string error = fmt::format("The ParticleAutocorrelation's points per cycle ({}) should larger than 0", points_per_cycle);
		throw std::runtime_error(error);
	}
}

ParticleAutocorrelation::~ParticleAutocorrelation() {

}

void ParticleAutocorrelation::_add_value(ullint time_diff, double value, std::map<ullint, uint> &n_conf) {
	// in some cases rounding issues will make time differences differ by 1 when instead they should coincide.
	// The following predicate looks for time differences that are equal or smaller than 1
	auto pred = [time_diff](const auto &other_diff) {
		return std::abs((long long int) time_diff - (long long int) other_diff.first) <= 1;
	};

	// we apply the above predicate to the map
	auto it = std::find_if(_result.begin(), _result.end(), pred);
	if(it == _result.end()) {
		_result[time_diff] = value;
		n_conf[time_diff] = 1;
	}
	// if a compatible time difference (i.e. the same or one that differs by 1) is found then we use that to update the data structures
	else {
		_result[it->first] += value;
		n_conf[it->first]++;
	}
}

void ParticleAutocorrelation::analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) {
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
			BA_INFO("Computing the autocorrelation on configurations composed of {} particles", N_conf);
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
				double value = _conf_conf_value(current_cycle_base, past_base);
				ullint time_diff = current_cycle_base->time - past_base->time;

				_add_value(time_diff, value, n_conf);
			}
		}

		double value = _conf_conf_value(current_cycle_base, frame);
		ullint time_diff = frame->time - current_cycle_base->time;

		_add_value(time_diff, value, n_conf);

		idx++;
		frame = trajectory->next_frame();
	}

	for(auto &pair : _result) {
		pair.second /= n_conf[pair.first];
	}

	if(_normalise) {
		double norm_by = _result[0];
		for(auto &pair : _result) {
			pair.second /= norm_by;
		}
	}
}

void ParticleAutocorrelation::analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file) {
	analyse_trajectory(trajectory);

	ofstream output(output_file);

	for(auto pair : _result) {
		output << pair.first << " " << pair.second << endl;
	}

	output.close();
}

double ParticleAutocorrelation::_conf_conf_value(std::shared_ptr<System> first, std::shared_ptr<System> second) {
	double value = 0.;
	uint N = first->N();
	vec3 com_diff(0., 0., 0.);
	for(uint i = 0; i < N; i++) {
		value += glm::dot(_accessor_function(second->particles()[i].get()), _accessor_function(first->particles()[i].get()));
	}

	return value / N;
}

#ifdef PYTHON_BINDINGS

void export_ParticleAutocorrelation(py::module &m) {
	py::class_<ParticleAutocorrelation, std::shared_ptr<ParticleAutocorrelation>> obs(m, "ParticleAutocorrelation", R"pb(
Compute the normalised autocorrelation of particle-related three-dimensional vectors.

The constructor takes a function that will be used on each particle to return the three-dimensional vector whose autocorrelation will be computed.
As an example, the two observables defined below will compute the velocity and angular velocity autocorrelation functions::

	v_obs = ba.ParticleAutocorrelation(30, lambda p: p.velocity)
	omega_obs = ba.ParticleAutocorrelation(30, lambda p: p.angular_velocity)

The trajectory is split up in chunks of size `points_per_cycle`, and the autocorrelation is computed between configurations in each chunk and
between the initial configurations of pairs of chunks.
)pb");

	obs.def(py::init<uint, AccessorType, bool>(), py::arg("points_per_cycle"), py::arg("function"), py::arg("normalise") = false, R"pb(
Parameters
----------
points_per_cycle: int
    The number of configurations contained in each chunk in which the trajectory is split up.
function : callable
    A callable that takes a particle and returns the three-dimensional vector whose autocorrelation will be computed.
normalise : bool
    If True, the the autocorrelation will be normalised (i.e. its value in 0 will be 1). Defaults to False.
)pb");

	obs.def("analyse_and_print", &ParticleAutocorrelation::analyse_and_print, py::arg("trajectory"), py::arg("output_file"), R"pb(
Analyse the trajectory and print the autocorrelation directly to the given file.
	)pb");

	PY_EXPORT_TRAJECTORY_OBS(obs, ParticleAutocorrelation);
}

#endif

} /* namespace ba */
