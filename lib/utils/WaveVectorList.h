/*
 * WaveVectorList.h
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#ifndef UTILS_WAVEVECTORLIST_H_
#define UTILS_WAVEVECTORLIST_H_

#include "../defs.h"
#include "../System.h"

#include <map>

namespace ba {

struct WaveVectorList {
public:
	WaveVectorList(const std::vector<double> &pq_modules, uint pmax_n_realisations);
	WaveVectorList(double pq_max, uint pmax_n_realisations, double pmax_delta_q);
	WaveVectorList(double pq_min, double pq_max, double pq_interval, uint pmax_n_realisations, double pmax_delta_q);
	virtual ~WaveVectorList();

	std::vector<double> q_modules();

	void init(std::shared_ptr<System> syst);
	void init(const vec3 box);

	std::map<double, std::vector<vec3>> q_vectors;

protected:
	bool _init_from_list = false;

	double _q_min = 0.0;
	double _q_max = 10.0;
	double _q_interval = 0.0;
	uint _max_n_realisations = 20;
	double _max_delta_q = 0.01;

	std::vector<double> _q_modules;
};

#ifdef PYTHON_BINDINGS

void export_WaveVectorList(py::module &m);

#endif

} /* namespace ba */

#endif /* UTILS_WAVEVECTORLIST_H_ */
