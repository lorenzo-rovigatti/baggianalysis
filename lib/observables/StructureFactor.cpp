/*
 * StructureFactor.cpp
 *
 *  Created on: 30 jul 2020
 *      Author: lorenzo
 */

#include "StructureFactor.h"

#include "../utils/Random.h"
#include <glm/gtx/norm.hpp>

namespace ba {

StructureFactor::StructureFactor(double largest_q, uint max_n_realisations, double max_delta_q) :
				SystemObservable<std::map<double, double>>(),
				_largest_q(largest_q),
				_max_n_realisations(max_n_realisations),
				_max_delta_q(max_delta_q) {

}

StructureFactor::~StructureFactor() {

}

void StructureFactor::_init_qs(std::shared_ptr<System> system) {
	if(_last_box != system->box) {
		_q_vectors.clear();
		_last_box = system->box;

		std::list<vec3> all_qs;
		double sqr_max_q = SQR(_largest_q);
		vec3 delta_q(2. * M_PI / _last_box.x, 2. * M_PI / _last_box.y, 2. * M_PI / _last_box.z);

		// we first generate all q vectors
		for(int nx = -_largest_q / delta_q.x; nx <= _largest_q / delta_q.x; nx++) {
			for(int ny = -_largest_q / delta_q.y; ny <= _largest_q / delta_q.y; ny++) {
				for(int nz = -_largest_q / delta_q.z; nz <= _largest_q / delta_q.z; nz++) {
					if(nx == 0 && ny == 0 && nz == 0) continue;

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
		double first_q = -1.;
		for(auto q_vector : all_qs) {
			double q_mod = glm::length(q_vector);
			if(first_q < 0.) {
				first_q = q_mod;
			}
			std::vector<vec3> &curr_q_list = _q_vectors[first_q];

			curr_q_list.push_back(q_vector);

			if(fabs(q_mod - first_q) > _max_delta_q) {
				// if the current group contains more than the maximum allowed number of realisations
				if(curr_q_list.size() > _max_n_realisations) {
					// we randomly shuffle its contents
					std::random_shuffle(curr_q_list.begin(), curr_q_list.end());
					// and throw away the last (size - _max_n_realisations) elements
					curr_q_list.erase(curr_q_list.begin() + _max_n_realisations, curr_q_list.end());
				}
				first_q = -1.;
			}
		}
	}
}

void StructureFactor::analyse_system(std::shared_ptr<System> system) {
	_init_qs(system);

	for(auto pair : _q_vectors) {
		double q_module = pair.first;
		std::vector<vec3> &q_vectors = pair.second;
		for(auto q_vector : q_vectors) {
			double sq_cos = 0.;
			double sq_sin = 0.;
			for(auto p: system->particles()) {
				vec3 r = p->position();
				double qr = glm::dot(q_vector, r);
				sq_cos += std::cos(qr);
				sq_sin += std::sin(qr);
			}

			_result[q_module] += (SQR(sq_cos) + SQR(sq_sin)) / system->N();
		}
		_result[q_module] /= q_vectors.size();
	}

	_times_called++;
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
	py::class_<StructureFactor, std::shared_ptr<StructureFactor>> obs(m, "StructureFactor");

	obs.def(py::init<double, uint, double>());

	PY_EXPORT_SYSTEM_OBS(obs, StructureFactor);
}

#endif

} /* namespace ba */
