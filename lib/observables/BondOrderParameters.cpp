/*
 * BondOrderParameters.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "BondOrderParameters.h"

#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <gsl/gsl_sf_coupling.h>

namespace ba {

BondOrderParameters::BondOrderParameters(std::set<int> orders_to_compute, bool compute_avg_qs, bool compute_qs, bool compute_avg_ws, bool compute_ws) :
				SystemObservable<std::vector<vector_scalar>>(),
				_orders_to_compute(orders_to_compute),
				_compute_avg_qs(compute_avg_qs),
				_compute_qs(compute_qs),
				_compute_avg_ws(compute_avg_ws),
				_compute_ws(compute_ws) {
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
		vector_scalar avg_qs, qs, avg_ws, ws;

		avg_qs.reserve(_orders_to_compute.size());
		qs.reserve(_orders_to_compute.size());
		avg_ws.reserve(_orders_to_compute.size());
		ws.reserve(_orders_to_compute.size());
		for(auto order : _orders_to_compute) {
			double ql_avg_sqr = 0.;
			double ql_sqr = 0.;
			std::vector<std::complex<double>> qlm_avg(2 * order + 1, 0.);
			for(int m = -order; m <= order; m++) {
				int m_idx = m + order;
				qlm_avg[m_idx] = bops[p->index()][order][m_idx];

				for(auto neigh : p->neighbours()) {
					auto q = neigh.other();
					qlm_avg[m_idx] += bops[q->index()][order][m_idx];
				}

				qlm_avg[m_idx] /= p->neighbours().size() + 1.;

				ql_avg_sqr += std::norm(qlm_avg[m_idx]);
				ql_sqr += std::norm(bops[p->index()][order][m_idx]);
			}

			if(_compute_avg_qs) {
				ql_avg_sqr *= (4. * M_PI) / (2. * order + 1.);
				avg_qs.emplace_back(std::sqrt(ql_avg_sqr));
			}

			if(_compute_qs) {
				ql_sqr *= (4. * M_PI) / (2. * order + 1.);
				qs.emplace_back(std::sqrt(ql_sqr));
			}

			if(_compute_avg_ws || _compute_ws) {
				// now we compute the w's
				double wl_avg = 0.;
				double wl = 0.;
				for(int m1 = -order; m1 <= order; m1++) {
					std::complex<double> qlm1 = bops[p->index()][order][m1 + order];
					std::complex<double> qlm1_avg = qlm_avg[m1 + order];
					for(int m2 = -order; m2 <= order; m2++) {
						int m3 = -m1 - m2;

						if(abs(m3) <= order) {
							std::complex<double> qlm2 = bops[p->index()][order][m2 + order];
							std::complex<double> qlm2_avg = qlm_avg[m2 + order];
							std::complex<double> qlm3 = bops[p->index()][order][m3 + order];
							std::complex<double> qlm3_avg = qlm_avg[m3 + order];

							double factor_3j = gsl_sf_coupling_3j(2 * order, 2 * order, 2 * order, 2 * m1, 2 * m2, 2 * m3);
							wl += factor_3j * std::real(qlm1 * qlm2 * qlm3);
							wl_avg += factor_3j * std::real(qlm1_avg * qlm2_avg * qlm3_avg);
						}
					}
				}
				if(_compute_avg_ws) {
					wl_avg /= pow(ql_avg_sqr, 1.5);
					avg_ws.emplace_back(wl_avg);
				}

				if(_compute_ws) {
					ws.emplace_back(wl);
				}
			}
		}

		res_it->reserve(avg_qs.size() + qs.size() + avg_ws.size() + ws.size());
		res_it->insert(res_it->begin(), avg_qs.begin(), avg_qs.end());
		res_it->insert(res_it->end(), qs.begin(), qs.end());
		res_it->insert(res_it->end(), avg_ws.begin(), avg_ws.end());
		res_it->insert(res_it->end(), ws.begin(), ws.end());

		res_it++;
	}
}

void BondOrderParameters::_set_particle_bops(std::shared_ptr<Particle> p, particle_bops &bops, const vec3 &box) {
	for(auto order : _orders_to_compute) {
		bops[order] = std::vector<std::complex<double>>(2 * order + 1, 0.);
	}

	int N_neighbours = p->neighbours().size();
	if(N_neighbours > 0) {
		for(auto neigh : p->neighbours()) {
			auto q = neigh.other();
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

	obs.def(py::init<std::set<int>, bool, bool, bool, bool>(),
			py::arg("orders_to_compute"),
			py::arg("compute_avg_qs") = true,
			py::arg("compute_qs") = false,
			py::arg("compute_avg_ws") = false,
			py::arg("compute_ws") = false);

	PY_EXPORT_SYSTEM_OBS(obs, BondOrderParameters);
}

#endif

} /* namespace ba */
