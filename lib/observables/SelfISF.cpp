/*
 * SelfISF.cpp
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#include "SelfISF.h"

namespace ba {

SelfISF::SelfISF(double largest_q, uint max_n_realisations, double max_delta_q, uint points_per_cycle) :
		TrajectoryObservable<std::map<ullint, std::vector<double>>>(),
		_points_per_cycle(points_per_cycle),
		_largest_q(largest_q),
		_max_n_realisations(max_n_realisations),
		_max_delta_q(max_delta_q) {
	if(points_per_cycle < 1) {
		std::string error = fmt::format("The number of points per cycle ({}) should larger than 0", points_per_cycle);
		throw std::runtime_error(error);
	}
}

SelfISF::~SelfISF() {

}

void SelfISF::_init_qs(std::shared_ptr<System> system) {
	if(_last_box != system->box) {
		_q_vectors.clear();
		_last_box = system->box;

		std::list<vec3> all_qs;
		double sqr_max_q = SQR(_largest_q);
		vec3 delta_q(2. * M_PI / _last_box.x, 2. * M_PI / _last_box.y, 2. * M_PI / _last_box.z);

		// we first generate all q vectors
		for(int nx = 0; nx <= _largest_q / delta_q.x; nx++) {
			for(int ny = -_largest_q / delta_q.y; ny <= _largest_q / delta_q.y; ny++) {
				for(int nz = -_largest_q / delta_q.z; nz <= _largest_q / delta_q.z; nz++) {
					// the following two conditions (together with the nx >= 0 conditions specified above)
					// make sure that we don't include pairs of q-vectors that differ only by a factor of -1
					if(nx == 0 && ny < 0) {
						continue;
					}
					if(nx == 0 && ny == 0 && nz <= 0) {
						continue;
					}

					vec3 new_q(delta_q);
					new_q.x *= nx;
					new_q.y *= ny;
					new_q.z *= nz;

					if(glm::dot(new_q, new_q)  <= sqr_max_q) {
						all_qs.push_back(new_q);
					}
				}
			}
		}

		// sort them according to their length
		auto sort_function = [](vec3 &q1, vec3 &q2) -> bool {
			return glm::dot(q1, q1) < glm::dot(q2, q2);
		};
		all_qs.sort(sort_function);

		// and then group them
		double first_q = -1;
		for(auto q_vector : all_qs) {
			double q_mod = glm::length(q_vector);

			if(fabs(q_mod - first_q) > _max_delta_q) {
				first_q = q_mod;
			}

			_q_vectors[first_q].push_back(q_vector);
		}
	}

	for(auto &q_pair : _q_vectors) {
		if(q_pair.second.size() > _max_n_realisations) {
			// we randomly shuffle its contents
			std::random_shuffle(q_pair.second.begin(), q_pair.second.end());
			// and throw away the last (size - _max_n_realisations) elements
			q_pair.second.erase(q_pair.second.begin() + _max_n_realisations, q_pair.second.end());
		}
	}
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

	for(auto &pair : _q_vectors) {
		std::vector<vec3> &q_list = pair.second;

		double q_self_isf = 0.;
		for(auto q_vector : q_list) {
			for(uint i = 0; i < first->N(); i++) {
				double qr = glm::dot(q_vector, diffs[i]);
				q_self_isf += std::cos(qr);
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
	_init_qs(frame); // this function uses _last_box: try to come up with a better way of handling this
	_add_value(0, std::vector<double>(_q_vectors.size(), 1.), n_conf); // at time t = 0 the correlation is always 1
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

#ifdef PYTHON_BINDINGS

void export_SelfISF(py::module &m) {
	py::class_<SelfISF, std::shared_ptr<SelfISF>> obs(m, "SelfISF", R"pb(
Compute the self part of the spatial Fourier transform of the van Hove function, also known as the intermediate scattering function (ISF) :math:`F_s(q, t)`, of a trajectory.

The trajectory is split up in chunks of size `points_per_cycle`, and the self ISF is computed between configurations in each chunk and
between the initial configurations of pairs of chunks for different wave vectors :math:`q` with modules that go from the smallest one (set by the box size), to a user-defined maximum.

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

	PY_EXPORT_TRAJECTORY_OBS(obs, SelfISF);
}

#endif

} /* namespace ba */
