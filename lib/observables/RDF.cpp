/*
 * RDF.cpp
 *
 *  Created on: 29 apr 2021
 *      Author: lorenzo
 */

#include "RDF.h"

#include <glm/gtx/component_wise.hpp>

#include <list>

namespace ba {

RDF::RDF(double bin_size) :
				SystemObservable<std::map<double, double>>(),
				_bin_size(bin_size) {
}

RDF::RDF(double bin_size, double max_value) :
				SystemObservable<std::map<double, double>>(),
				_bin_size(bin_size),
				_max_value_sqr(SQR(max_value)) {
}

RDF::~RDF() {

}

void RDF::analyse_system(std::shared_ptr<System> system) {
	double smallest_side = glm::compMin(system->box);
	double new_max_value = smallest_side / 2.;
	if(_max_value == 0.) {
		_max_value = new_max_value;
		_max_value_sqr = SQR(_max_value);
		int nbins = (int) (floor(_max_value / _bin_size) + 0.01);
		_bin_size = _max_value / nbins;
		_profile.resize(nbins);
	}
	else if(_max_value != new_max_value) {
		BA_WARNING("RDF: the smallest side of the first and current configuration (time: {}) differ ({} != {}). This could have some nasty effects on the values of the g(r) at long distances", system->time, _max_value * 2., smallest_side);
	}

	_times_called++;

	int n_pairs = 0;
	for(uint i = 0; i < system->N(); i++) {
		auto p = system->particles()[i];
		for(uint j = 0; j < i; j++) {
			auto q = system->particles()[j];
			n_pairs++;
		}
	}

	double factor = (system->box[0] * system->box[1] * system->box[2]) / n_pairs;
	for(uint i = 0; i < system->N(); i++) {
		auto p = system->particles()[i];
		for(uint j = 0; j < i; j++) {
			auto q = system->particles()[j];
			vec3 distance = q->position() - p->position();
			// periodic boundary conditions
			distance -= glm::round(distance / system->box) * system->box;
			double distance_sqr = glm::dot(distance, distance);
			if(distance_sqr < _max_value_sqr) {
				int mybin = (int) (std::floor(std::sqrt(distance_sqr) / _bin_size) + 0.001);
				_profile[mybin] += factor;
			}
		}
	}
}

std::map<double, double> RDF::_finalised_result() {
	_result.clear();

	double myx = _bin_size / 2.;
	double norm_factor = 4 * M_PI * _times_called * _bin_size;
	for(auto value: _profile) {
		_result[myx] = value / (norm_factor * myx * myx);
		myx += _bin_size;
	}

	return _result;
}

#ifdef PYTHON_BINDINGS

void export_RDF(py::module &m) {
	py::class_<RDF, std::shared_ptr<RDF>> obs(m, "RDF", "Compute the `radial distribution function <https://en.wikipedia.org/wiki/Radial_distribution_function>`_ of a system.");

	obs.def(py::init<double>());
	obs.def(py::init<double, double>());

	PY_EXPORT_SYSTEM_OBS(obs, RDF);
}

#endif

} /* namespace ba */
