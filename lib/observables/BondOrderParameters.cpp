/*
 * BondOrderParameters.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "BondOrderParameters.h"

#include <boost/math/special_functions/spherical_harmonic.hpp>

namespace ba {

BondOrderParameters::BondOrderParameters(std::set<int> orders_to_compute) :
				SystemObservable<std::vector<vector_scalar>>(),
				_orders_to_compute(orders_to_compute) {

}

BondOrderParameters::~BondOrderParameters() {

}

void BondOrderParameters::analyse_system(std::shared_ptr<System> frame) {
	// first we compute the bond-order parameters for each particle
	std::map<int, particle_bops> bops;
	for(auto p : frame->particles()) {
		bops[p->index()] = particle_bops();
		_set_particle_bops(p, bops[p->index()], frame->box);
	}

	_result.resize(frame->N());
	auto res_it = _result.begin();

	for(auto p : frame->particles()) {
		for(auto order : _orders_to_compute) {
			double ql_sqr_avg = 0.;
			for(int m = -order; m <= order; m++) {
				int m_idx = m + order;
				std::complex<double> qlm_avg = bops[p->index()][order][m_idx];

				for(auto q : p->neighbours()) {
					qlm_avg += bops[q->index()][order][m_idx];
				}

				qlm_avg /= p->neighbours().size() + 1.;
				ql_sqr_avg += std::norm(qlm_avg);
			}

			ql_sqr_avg *= (4. * M_PI) / (2. * order + 1.);
			res_it->push_back(std::sqrt(ql_sqr_avg));
		}

		res_it++;
	}
}

void BondOrderParameters::_set_particle_bops(std::shared_ptr<Particle> p, particle_bops &bops, const vec3 &box) {
	for(auto order : _orders_to_compute) {
		bops[order] = std::vector<std::complex<double>>(2 * order + 1, 0.);
	}

	int N_neighbours = p->neighbours().size();
	if(N_neighbours > 0) {
		for(auto q : p->neighbours()) {
			vec3 distance = p->position() - q->position();
			// periodic boundary conditions
			distance -= glm::round(distance / box) * box;

			double theta = std::acos(distance[2] / glm::length(distance));
			double phi = std::atan2(distance[1], distance[0]);
			if(phi < 0.) {
				phi += 2*M_PI;
			}

			for(auto order : _orders_to_compute) {
				for(int m = -order; m <= order; m++) {
					int m_idx = m + order;

					bops[order][m_idx] += boost::math::spherical_harmonic(order, m, theta, phi);
				}
			}
		}

		for(auto order : _orders_to_compute) {
			for(auto &v : bops[order]) {
				v /= N_neighbours;
			}
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_BondOrderParameters(py::module &m) {
	py::class_<BondOrderParameters, std::shared_ptr<BondOrderParameters>> obs(m, "BondOrderParameters");

	obs
		.def(py::init<std::set<int>>());

	PY_EXPORT_SYSTEM_OBS(obs, BondOrderParameters);
}

#endif

} /* namespace ba */
