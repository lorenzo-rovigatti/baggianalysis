/*
 * StructureFactor.cpp
 *
 *  Created on: 30 jul 2020
 *      Author: lorenzo
 */

#include "StructureFactor.h"

#include "../utils/Random.h"
#include <glm/gtx/norm.hpp>

#include <list>

namespace ba {

StructureFactor::StructureFactor(double largest_q, uint max_n_realisations, double max_delta_q) :
				SystemObservable<std::map<double, double>>(),
				_largest_q(largest_q),
				_max_n_realisations(max_n_realisations),
				_max_delta_q(max_delta_q) {

}

StructureFactor::~StructureFactor() {

}

void StructureFactor::clear_b_factors() {
	_b_factors.clear();
}

void StructureFactor::set_b_factors(vector_scalar b_factors) {
	_b_factors = b_factors;
}

DestructuredStructureFactor StructureFactor::destructured_from_system(std::shared_ptr<System> system) {
	DestructuredStructureFactor result;

	_init_qs(system);

	// find the q-module containing the largest number of q vectors
	uint largest_q_size = 0;
	for(auto pair : _q_vectors) {
		if(pair.second.size() > largest_q_size) {
			largest_q_size = pair.second.size();
		}
	}

	for(auto pair : _q_vectors) {
		double q_module = pair.first;
		std::vector<vec3> &q_vectors = pair.second;
		double norm = std::sqrt(q_vectors.size() * system->N());

		result.q_modules.push_back(q_module);

		std::vector<vector_scalar> q_cos_contribs(largest_q_size, vector_scalar(system->N()));
		std::vector<vector_scalar> q_sin_contribs(largest_q_size, vector_scalar(system->N()));

		int q_idx = 0;
		for(auto q_vector : q_vectors) {
			for(uint i = 0; i < system->N(); i++) {
				auto p = system->particles()[i];

				vec3 r = p->position();
				double qr = glm::dot(q_vector, r);

				q_cos_contribs[q_idx][i] += std::cos(qr) / norm;
				q_sin_contribs[q_idx][i] += std::sin(qr) / norm;
			}

			q_idx++;
		}

		result.cos_contribs.push_back(q_cos_contribs);
		result.sin_contribs.push_back(q_sin_contribs);
	}

	return result;
}

void StructureFactor::analyse_system(std::shared_ptr<System> system) {
	_init_qs(system);

	bool has_b_factors = _b_factors.size() > 0;

	if(has_b_factors && _b_factors.size() != system->N()) {
		std::string error = fmt::format("StructureFactor: The size of the b-factor vector ({}) is different from the number of particles ({})", _b_factors.size(), system->N());
		throw std::runtime_error(error);
	}

	for(auto pair : _q_vectors) {
		double q_module = pair.first;
		std::vector<vec3> &q_vectors = pair.second;
		for(auto q_vector : q_vectors) {
			double sq_cos = 0.;
			double sq_sin = 0.;
			for(uint i = 0; i < system->N(); i++) {
				auto p = system->particles()[i];
				double b_factor = (has_b_factors) ? _b_factors[i] : 1;

				vec3 r = p->position();
				double qr = glm::dot(q_vector, r);
				sq_cos += b_factor * std::cos(qr);
				sq_sin += b_factor * std::sin(qr);
			}

			_result[q_module] += (SQR(sq_cos) + SQR(sq_sin)) / (system->N() * q_vectors.size());
		}
	}

	_times_called++;
}

void StructureFactor::_init_qs(std::shared_ptr<System> system) {
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
				if(first_q != -1) {
					std::vector<vec3> &curr_q_list = _q_vectors[first_q];
					if(curr_q_list.size() > _max_n_realisations) {
						// we randomly shuffle its contents
						std::random_shuffle(curr_q_list.begin(), curr_q_list.end());
						// and throw away the last (size - _max_n_realisations) elements
						curr_q_list.erase(curr_q_list.begin() + _max_n_realisations, curr_q_list.end());
					}
				}

				first_q = q_mod;
			}

			_q_vectors[first_q].push_back(q_vector);
		}
	}
}

std::map<double, double> StructureFactor::_finalised_result() {
	std::map<double, double> result(_result);
	for(auto &p: result) {
		p.second /= _times_called;
	}
	return result;
}

#ifdef PYTHON_BINDINGS

void export_StructureFactor(py::module &m) {
	py::class_<DestructuredStructureFactor, std::shared_ptr<DestructuredStructureFactor>> destr_sf(m, "DestructuredStructureFactor");

	destr_sf.def(py::init<>());
	destr_sf.def_readwrite("q_modules", &DestructuredStructureFactor::q_modules);
	destr_sf.def_readwrite("sin_contribs", &DestructuredStructureFactor::sin_contribs);
	destr_sf.def_readwrite("cos_contribs", &DestructuredStructureFactor::cos_contribs);

	py::class_<StructureFactor, std::shared_ptr<StructureFactor>> obs(m, "StructureFactor", "Compute the `structure factor <https://en.wikipedia.org/wiki/Structure_factor>`_ of a system.");

	obs.def(py::init<double, uint, double>());

	obs.def("clear_b_factors", &StructureFactor::clear_b_factors, R"pbdoc(
		Reset the b-factors to their default values (b = 1 for each particle).
	)pbdoc");

	obs.def("set_b_factors", &StructureFactor::set_b_factors, py::arg("b_factors"), R"pbdoc(
		Set the particles' b-factors (that is, the scattering amplitudes) that will be used to compute the structure factor.

		Parameters
		----------
		b_factors : List(float)
			The vector of b-factors to be used in the calculation of the S(q). The length of the vector should be equal to the number of particles in the system.
	)pbdoc");

	obs.def("destructured_from_system", &StructureFactor::destructured_from_system, py::arg("system"), R"pbdoc(
		Compute and return the single-particle contributions (splitted as cosine and sine contributions stored in a :class:`DestructuredStructureFactor` instance) that make up the structure factor.

        Parameters
        ----------
		system : :class:`System`
			The input system.

        Returns
        -------
        :class:`DestructuredStructureFactor`
            The final result of the computation.
	)pbdoc");

	PY_EXPORT_SYSTEM_OBS(obs, StructureFactor);
}

#endif

} /* namespace ba */
