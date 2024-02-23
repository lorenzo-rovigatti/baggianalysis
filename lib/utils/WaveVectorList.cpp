/*
 * WaveVectorList.cpp
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#include "WaveVectorList.h"

namespace ba {

WaveVectorList::WaveVectorList(const std::vector<double> &pq_modules, uint pmax_n_realisations) :
				_init_from_list(true),
				_max_n_realisations(pmax_n_realisations),
				_q_modules(pq_modules) {
	std::sort(_q_modules.begin(), _q_modules.end());
	_q_max = _q_modules.back();
}

WaveVectorList::WaveVectorList(double pq_max, uint pmax_n_realisations, double pmax_delta_q) :
				_q_max(pq_max),
				_max_n_realisations(pmax_n_realisations),
				_max_delta_q(pmax_delta_q) {

}

WaveVectorList::WaveVectorList(double pq_min, double pq_max, double pq_interval, uint pmax_n_realisations, double pmax_delta_q) :
				_q_min(pq_min),
				_q_max(pq_max),
				_q_interval(pq_interval),
				_max_n_realisations(pmax_n_realisations),
				_max_delta_q(pmax_delta_q) {

}

WaveVectorList::~WaveVectorList() {

}

void WaveVectorList::init(std::shared_ptr<System> syst) {
	init(syst->box);
}

void WaveVectorList::init(const vec3 box) {
	q_vectors.clear();

	std::list<vec3> all_qs;
	double sqr_max_q = SQR(_q_max);
	vec3 delta_q(2. * M_PI / box);

	// we first generate all q vectors
	for(int nx = 0; nx <= _q_max / delta_q.x; nx++) {
		for(int ny = -_q_max / delta_q.y; ny <= _q_max / delta_q.y; ny++) {
			for(int nz = -_q_max / delta_q.z; nz <= _q_max / delta_q.z; nz++) {
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

				if(glm::dot(new_q, new_q) <= sqr_max_q) {
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

		q_vectors[first_q].push_back(q_vector);
	}

	// clean up the list so that only the q vectors that the user wants are retained
	if(_init_from_list) { // use _q_modules
		std::map<double, std::vector<vec3>> filtered_q_vectors;
		auto current_q_modules = q_modules();

		if(_q_modules.front() < current_q_modules.front()) {
			BA_WARNING("The smallest q vector in the user-supplied list (%lf) is smaller than the smallest possible q vector (%lf)", _q_modules.front(), current_q_modules.front());
		}

		for(auto &q : _q_modules) {
			double closest_q = *std::min_element(std::begin(current_q_modules), std::end(current_q_modules), [=] (double x, double y) {
				return std::abs(x - q) < std::abs(y - q);
			});

			filtered_q_vectors[closest_q] = q_vectors.at(closest_q);
		}

		q_vectors.clear();
		q_vectors.insert(filtered_q_vectors.begin(), filtered_q_vectors.end());

	}
	else { // use q_min, q_max and _q_interval
		double last_retained_q = -1.0;
		for(auto q_pair = std::begin(q_vectors); q_pair != std::end(q_vectors);) {
			// discard all those q vectors with q < _q_min
			if(q_pair->first < _q_min) {
				q_pair = q_vectors.erase(q_pair);
				continue;
			}

			// if q_interval has been set, retain only q vectors that are separated by (roughly) _q_interval
			if(last_retained_q < 0.) {
				last_retained_q = q_pair->first;
			}
			else if(_q_interval > 0.) {
				if((q_pair->first - last_retained_q) < _q_interval) {
					q_pair = q_vectors.erase(q_pair);
					continue;
				}
				last_retained_q = q_pair->first;
			}

			++q_pair;
		}
	}

	for(auto &q_pair : q_vectors) {
		// randomly pick max_n_realisations for each surviving q
		if(q_pair.second.size() > _max_n_realisations) {
			// we randomly shuffle its contents
			std::random_shuffle(q_pair.second.begin(), q_pair.second.end());
			// and throw away the last (size - _max_n_realisations) elements
			q_pair.second.erase(q_pair.second.begin() + _max_n_realisations, q_pair.second.end());
		}
	}
}

std::vector<double> WaveVectorList::q_modules() {
	std::vector<double> keys;

	for(auto &q_pair: q_vectors) {
		keys.push_back(q_pair.first);
	}

	return keys;
}

void export_WaveVectorList(py::module &m) {
	py::class_<WaveVectorList, std::shared_ptr<WaveVectorList>> obs(m, "WaveVectorList", R"pb(
A class storing list of q-vectors that are used to compute correlation functions in Fourier space (see *e.g.* :class:`StructureFactor` or :class:`SelfISF`).
)pb");

	obs.def(py::init<double, uint, double>(), py::arg("q_max"), py::arg("max_n_realisations"), py::arg("max_delta_q"), R"pb(
Parameters
----------
q_max: float
    The length of the largest q vector of interest.
max_n_realisations: int
    Given a length :math:`q`, this is the maximum number of q vectors that will be averaged over to compute :math:`F_s(q, t)`.
max_delta_q: float
    q-vectors that are separated by distances smaller than this value will be assigned to the same value of :math:`q`.
)pb");

	obs.def(py::init<double, double, double, uint, double>(), py::arg("q_min"), py::arg("q_max"), py::arg("q_interval"), py::arg("max_n_realisations"), py::arg("max_delta_q"), R"pb(
Parameters
----------
q_min: float
    The length of the smallest q vector of interest.
q_max: float
    The length of the largest q vector of interest.
q_interval: float
    The distance between two neighbouring q-vectors in the final list. All the q-vectors in-between will be discarded.
max_n_realisations: int
    Given a length :math:`q`, this is the maximum number of q vectors that will be averaged over to compute :math:`F_s(q, t)`.
max_delta_q: float
    q-vectors that are separated by distances smaller than this value will be assigned to the same value of :math:`q`.
)pb");

	obs.def(py::init<const std::vector<double> &, uint>(), py::arg("q_modules"), py::arg("max_n_realisations"), R"pb(
Supply a list of q modules that will be used to initialise the list of q-vectors contained in this instance after initialisation. 
Note that the final q-vectors will depend on the box size of the system that will be used to initialise the instance, so that their 
modules will slightly differ from the ones contained in :attr:`q_modules`. 

Parameters
----------
q_modules: list(float)
    The q modules that will be included in the final q-vector list.
max_n_realisations: int
    Given a length :math:`q`, this is the maximum number of q vectors that will be averaged over to compute :math:`F_s(q, t)`.
)pb");

	obs.def("init", (void (WaveVectorList::*)(std::shared_ptr<System>))(&WaveVectorList::init), py::arg("system"), R"pb(
Initialise the list of q-vectors using the box size of the given system.

Parameters
----------
system : :class:`System`
    The input system whose simulation box the q vectors will be based on.
)pb");

	obs.def("q_modules", &WaveVectorList::q_modules, R"pb(
A list of numbers storing the modules of the q vectors.
)pb");

	obs.def_readwrite("q_vectors", &WaveVectorList::q_vectors, "A dictionary storing all the q vectors associated to this instance.");
}

} /* namespace ba */
