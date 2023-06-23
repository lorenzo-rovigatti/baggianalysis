/*
 * SelfISF.cpp
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#include "SelfISF.h"

#include "../utils/math.h"

namespace ba {

SelfISF::SelfISF(double largest_q, uint max_n_realisations, double max_delta_q, uint points_per_cycle) :
		TrajectoryObservable<std::map<ullint, std::vector<double>>>(),
		_points_per_cycle(points_per_cycle),
		_q_list(largest_q, max_n_realisations, max_delta_q) {
	if(points_per_cycle < 1) {
		std::string error = fmt::format("The number of points per cycle ({}) should larger than 0", points_per_cycle);
		throw std::runtime_error(error);
	}
}

SelfISF::SelfISF(const WaveVectorList &q_vectors, uint points_per_cycle) :
	_points_per_cycle(points_per_cycle),
	_q_list(q_vectors) {

}

SelfISF::~SelfISF() {

}

void SelfISF::_add_value(ullint time_diff, std::vector<double> values, std::map<ullint, uint> &n_conf) {
	// in some cases rounding issues will make time differences differ by 1 when instead they should coincide.
	// The following predicate looks for time differences that are equal or smaller than 1
	auto pred = [time_diff](const auto &other_diff) {
		return std::abs((long long int) time_diff - (long long int) other_diff.first) <= 1;
	};

	// we apply the above predicate to the map
	auto it = std::find_if(_result.begin(), _result.end(), pred);
	if(it == _result.end()) {
		_result[time_diff] = values;
		n_conf[time_diff] = 1;
	}
	// if a compatible time difference (i.e. the same or one that differs by 1) is found then we use that to update the data structures
	else {
		// sum the two vectors element wise, storing the result into the _result[it->first] vector
		std::transform(values.begin(), values.end(), _result[it->first].begin(), _result[it->first].begin(), std::plus<double>());
		n_conf[it->first]++;
	}
}

std::vector<double> SelfISF::_conf_conf_SelfISF(std::shared_ptr<System> first, std::shared_ptr<System> second) {
	std::vector<double> values;

	vec3 com_diff = second->com() - first->com();

	// here we compute the distance between particles
	std::vector<vec3> diffs;
	diffs.reserve(first->N());
	auto binary_op = [com_diff](std::shared_ptr<Particle> p, std::shared_ptr<Particle> q) {
		return q->position() - p->position() - com_diff;
	};
	std::transform(first->particles().begin(), first->particles().end(), second->particles().begin(), diffs.begin(), binary_op);

	for(auto &pair : _q_list.q_vectors) {
		std::vector<vec3> &q_list = pair.second;

		double q_self_isf = 0.;
		for(auto q_vector : q_list) {
			for(uint i = 0; i < first->N(); i++) {
				double qr = glm::dot(q_vector, diffs[i]);
				q_self_isf += utils::cos(qr);
			}
		}
		q_self_isf /= (first->N() * q_list.size());
		values.push_back(q_self_isf);
	}

	return values;
}

void SelfISF::analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) {
	std::map<ullint, uint> n_conf;

	std::shared_ptr<System> current_cycle_base;
	std::vector<std::shared_ptr<System>> past_cycle_bases;

	uint N_first_conf = 0;
	uint idx = 0;

	auto frame = trajectory->next_frame();
	_q_list.init(frame);
	_add_value(0, std::vector<double>(_q_list.q_vectors.size(), 1.), n_conf); // at time t = 0 the correlation is always 1
	while(frame != nullptr) {
		uint N_conf = frame->N();
		if(N_first_conf == 0) {
			N_first_conf = N_conf;
			BA_INFO("Computing the self intermediate scattering function on configurations composed of {} particles", N_conf);
		}

		if(N_conf != N_first_conf) {
			std::string error = fmt::format("The configurations at times 0 and {} contain different number of particles (%d != {})", frame->time, N_first_conf, N_conf);
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
				std::vector<double> new_values = _conf_conf_SelfISF(current_cycle_base, past_base);
				ullint time_diff = current_cycle_base->time - past_base->time;

				_add_value(time_diff, new_values, n_conf);
			}
		}

		// don't compute the ISF of a configuration with itself
		if(current_cycle_base != frame) {
			std::vector<double> new_values = _conf_conf_SelfISF(current_cycle_base, frame);
			ullint time_diff = frame->time - current_cycle_base->time;

			_add_value(time_diff, new_values, n_conf);
		}

		idx++;
		frame = trajectory->next_frame();
	}

	for(auto &pair : _result) {
		if(pair.first > 0) {
			for(auto &value: pair.second) {
				value /= n_conf[pair.first];
			}
		}
	}
}

void SelfISF::analyse_and_print(std::shared_ptr<BaseTrajectory> trajectory, std::string output_file) {
	analyse_trajectory(trajectory);

	std::ofstream output(output_file);

	auto q_modules = _q_list.q_modules();
	output << "# time";
	for(auto q : q_modules) {
		output << " " << q;
	}
	output << std::endl;

	for(auto &pair : _result) {
		output << pair.first;
		for(auto &cf : pair.second) {
			output << " " << cf;
		}
		output << std::endl;
	}

	output.close();
}

#ifdef PYTHON_BINDINGS

void export_SelfISF(py::module &m) {
	py::class_<SelfISF, std::shared_ptr<SelfISF>> obs(m, "SelfISF", R"pb(
Compute the self part of the spatial Fourier transform of the van Hove function, also known as the intermediate scattering function (ISF) :math:`F_s(q, t)`, of a trajectory.

The trajectory is split up in chunks of size `points_per_cycle`, and the self ISF is computed between configurations in each chunk and
between the initial configurations of pairs of chunks for different wave vectors :math:`q`.

The self ISF between two configurations is associated to their time delta (*i.e.* to the difference between the times at
which they have been printed) and averaged with all the other pairs that share the same time delta. Note that in same cases (*i.e.* log or log-linear 
spacing), there might be time deltas that differ by one: in this case the code consider those pairs to share the same time delta.   
)pb");

	obs.def(py::init<double, uint, double, uint>(), py::arg("largest_q"), py::arg("max_n_realisations"), py::arg("max_delta_q"), py::arg("points_per_cycle"), R"pb(
Parameters
----------
largest_q: float
    The length of the largest q vector of interest
max_n_realisations: int
    Given a length :math:`q`, this is the maximum number of q vectors that will be averaged over to compute :math:`F_s(q, t)`
max_delta_q: float
    q-vectors that are separated by distances smaller than this value will be assigned to the same value of :math:`q`
points_per_cycle: int
    The number of configurations contained in each chunk in which the trajectory is split up.
)pb");

	obs.def(py::init<WaveVectorList &, uint>(), py::arg("q_vectors"), py::arg("max_n_realisations"), R"pb(
Parameters
----------
q_vectors: :class:`WaveVectorList`
	The list of q vectors that will be used to compute the self ISF.
points_per_cycle: int
	The number of configurations contained in each chunk in which the trajectory is split up.
)pb");

	obs.def("analyse_and_print", &SelfISF::analyse_and_print, py::arg("trajectory"), py::arg("output_file"), R"pb(
Analyse the trajectory and print the self ISF directly to the given file.
)pb");

	PY_EXPORT_TRAJECTORY_OBS(obs, SelfISF);
}

#endif

} /* namespace ba */
