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

struct WaveVectorList : public std::map<double, std::vector<vec3>> {
public:
	WaveVectorList();
	virtual ~WaveVectorList();

	void init_from_system(std::shared_ptr<System> syst, double largest_q, uint max_n_realisations, double max_delta_q);
};

} /* namespace ba */

#endif /* UTILS_WAVEVECTORLIST_H_ */
