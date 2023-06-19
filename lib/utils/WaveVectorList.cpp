/*
 * WaveVectorList.cpp
 *
 *  Created on: Jun 19, 2023
 *      Author: lorenzo
 */

#include "WaveVectorList.h"

namespace ba {

WaveVectorList::WaveVectorList() {

}

WaveVectorList::~WaveVectorList() {

}

void WaveVectorList::init_from_system(std::shared_ptr<System> syst, double largest_q, uint max_n_realisations, double max_delta_q) {
	this->clear();

	std::list<vec3> all_qs;
	double sqr_max_q = SQR(largest_q);
	vec3 delta_q(2. * M_PI / syst->box);

	// we first generate all q vectors
	for(int nx = 0; nx <= largest_q / delta_q.x; nx++) {
		for(int ny = -largest_q / delta_q.y; ny <= largest_q / delta_q.y; ny++) {
			for(int nz = -largest_q / delta_q.z; nz <= largest_q / delta_q.z; nz++) {
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

		if(fabs(q_mod - first_q) > max_delta_q) {
			first_q = q_mod;
		}

		(*this)[first_q].push_back(q_vector);
	}

	for(auto &q_pair : (*this)) {
		if(q_pair.second.size() > max_n_realisations) {
			// we randomly shuffle its contents
			std::random_shuffle(q_pair.second.begin(), q_pair.second.end());
			// and throw away the last (size - _max_n_realisations) elements
			q_pair.second.erase(q_pair.second.begin() + max_n_realisations, q_pair.second.end());
		}
	}
}

} /* namespace ba */
