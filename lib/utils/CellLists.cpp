/*
 * CellLists.cpp
 *
 *  Created on: Aug 10, 2019
 *      Author: lorenzo
 */

#include "CellLists.h"

#include <cfloat>
#include <glm/gtx/component_wise.hpp>

namespace ba {

CellLists::CellLists(bool init_shifts) :
				_init_shifts(init_shifts) {

}

CellLists::~CellLists() {

}

void CellLists::init_cells(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box, double rc) {
	_curr_box = box;

	if(box.x <= 0. || box.y <= 0. || box.z <= 0.) {
		std::string error = fmt::format("Cannot build cells: invalid simulation box {}, {}, {}", box.x, box.y, box.z);
		throw std::runtime_error(error);
	}

	next.clear();
	heads.clear();
	_cell_shifts.clear();

	N_cells_side = glm::floor(box / rc);
	for(int i = 0; i < 3; i++) {
		// if shifts are required, everything becomes easier if the number of cells per side is odd
		if(_init_shifts && ( N_cells_side[i] % 2) == 0) {
			N_cells_side[i]--;
		}
		if(N_cells_side[i] < 3) {
			N_cells_side[i] = 3;
		}
	}
	int N_cells = N_cells_side[0] * N_cells_side[1] * N_cells_side[2];
	next.resize(particles.size(), -1);
	heads.resize(N_cells, -1);

	for(size_t i = 0; i < particles.size(); i++) {
		auto p = particles[i];
		int cell_idx = get_cell_index(p->position());
		next[i] = heads[cell_idx];
		heads[cell_idx] = i;
	}

	// if the user asked for it, here we generate lists of all possible shifts according to their order (the shell around the central cell they belong to)
	if(_init_shifts) {
		int max_shift = (glm::compMax(N_cells_side) - 1) / 2;
		_cell_shifts.resize(max_shift);
		vec3 shift;
		for(shift[0] = -N_cells_side[0] / 2; shift[0] <= N_cells_side[0] / 2; shift[0]++) {
			for(shift[1] = -N_cells_side[1] / 2; shift[1] <= N_cells_side[1] / 2; shift[1]++) {
				for(shift[2] = -N_cells_side[2] / 2; shift[2] <= N_cells_side[2] / 2; shift[2]++) {
					auto shift_abs = glm::abs(shift);
					int shift_order = glm::compMax(shift_abs) - 1;
					if(shift_order == -1) {
						shift_order = 0;
					}
					_cell_shifts[shift_order].push_back(shift);
				}
			}
		}

		BA_DEBUG("Number of cells: {}, cells per box side: <{}, {}, {}>, maximum shift order: {}", N_cells, N_cells_side[0], N_cells_side[1], N_cells_side[2], max_shift);
	}
}

int CellLists::get_cell_index(const vec3 &pos) const {
	int res = (int) ((pos[0] / _curr_box[0] - std::floor(pos[0] / _curr_box[0])) * (1.0 - DBL_EPSILON) * N_cells_side[0]);
	res += N_cells_side[0] * ((int) ((pos[1] / _curr_box[1] - std::floor(pos[1] / _curr_box[1])) * (1.0 - DBL_EPSILON) * N_cells_side[1]));
	res += N_cells_side[0] * N_cells_side[1] * ((int) ((pos[2] / _curr_box[2] - std::floor(pos[2] / _curr_box[2])) * (1.0 - DBL_EPSILON) * N_cells_side[2]));
	return res;
}

glm::ivec3 CellLists::get_cell(const vec3 &pos) const {
	int cell_index = get_cell_index(pos);
	return glm::ivec3(cell_index % N_cells_side[0], (cell_index / N_cells_side[0]) % N_cells_side[1], cell_index / (N_cells_side[0] * N_cells_side[1]));
}

const std::vector<std::vector<glm::ivec3>> &CellLists::cell_shifts() const {
	if(!_init_shifts) {
		throw std::runtime_error("Cell shifts non initialised");
	}
	return _cell_shifts;
}

} /* namespace ba */
