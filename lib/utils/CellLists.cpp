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

void CellLists::add_particle(std::shared_ptr<Particle> p, size_t next_idx) {
	if(next_idx > next.size()) {
		if(next.size() == next.capacity()) {
			next.reserve(next.capacity() * 2);
		}
		next.resize(next.size() + 1, -1);
	}

	int cell_idx = get_cell_index(p->position());
	next[next_idx] = heads[cell_idx];
	heads[cell_idx] = next_idx;
}

void CellLists::init_cells(const std::vector<std::shared_ptr<Particle>> &particles, const vec3 &box, double rc) {
	_curr_box = box;

	if(box.x <= 0. || box.y <= 0. || box.z <= 0.) {
		std::string error = fmt::format("Cannot build cells: invalid simulation box {}, {}, {}", box.x, box.y, box.z);
		throw std::runtime_error(error);
	}

	if(particles.size() == 0) {
		std::string error = fmt::format("Cannot build cells: the particle vector cannot be empty");
		throw std::runtime_error(error);
	}

	next.clear();
	heads.clear();
	_cell_shifts.clear();

	N_cells_side = glm::floor(box / rc);
	_cell_size = box / vec3(N_cells_side);
	_smallest_cell_size = glm::min(glm::min(_cell_size[0], _cell_size[1]), _cell_size[2]);
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
	// this wastes a bit of memory but makes subsequent additions less expensive
	next.reserve(particles.size() * 2);
	next.resize(particles.size(), -1);
	heads.resize(N_cells, -1);

	for(size_t i = 0; i < particles.size(); i++) {
		auto p = particles[i];
		add_particle(p, i);
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

bool CellLists::is_overlap(const std::vector<std::shared_ptr<Particle>> &particles, const vec3 &pos, double cutoff) const {
	int largest_order = std::ceil(cutoff / _smallest_cell_size);
	auto shifts = cell_shifts();
	if((uint) largest_order >= shifts.size()) {
		largest_order = shifts.size() - 1;
	}

	double cutoff_sqr = SQR(cutoff);
	auto p_cell = get_cell(pos);
	for(int order = 0; order <= largest_order; order++) {
		for(auto shift : shifts[order]) {
			auto cell = p_cell + shift;
			cell[0] = (cell[0] + N_cells_side[0]) % N_cells_side[0];
			cell[1] = (cell[1] + N_cells_side[1]) % N_cells_side[1];
			cell[2] = (cell[2] + N_cells_side[2]) % N_cells_side[2];
			int cell_idx = cell[0] + N_cells_side[0] * (cell[1] + cell[2] * N_cells_side[1]);

			int current = heads[cell_idx];
			while(current != -1) {
				auto q = particles[current];

				vec3 distance = pos - q->position();
				// periodic boundary conditions
				distance -= glm::round(distance / _curr_box) * _curr_box;
				double distance_sqr = glm::dot(distance, distance);

				if(distance_sqr < cutoff_sqr) {
					return true;
				}

				current = next[current];
			}
		}
	}

	return false;
}

#ifdef PYTHON_BINDINGS

void export_CellLists(py::module &m) {
	py::class_<CellLists, std::shared_ptr<CellLists>> cells(m, "CellLists", R"pbdoc(
A data structure to manage cell lists of particles in a cubic box.
)pbdoc");

	cells.def(py::init<bool>(), py::arg("init_shifts"), R"pbdoc(
The constructor takes one mandatory arguments.

Parameters
----------
    init_shifts : bool
        Initialise the cell shifts. Defaults to False.
)pbdoc");

	cells.def("add_particle", &CellLists::add_particle, py::arg("p"), py::arg("next_idx"), R"pbdoc(
Add a particle to the cells.

Parameters
----------
p : :class:`Particle`
	The particle to be added.
next_idx: int
	The id used to index the particle internally. In most cases this is just the index of the particle.
)pbdoc");

	cells.def("init_cells", &CellLists::init_cells, py::arg("particles"), py::arg("box"), py::arg("rc"), R"pbdoc(
Initial the cells data structures.

Parameters
----------
particles : List(:class:`Particle`)
	The particles that will be used to build the cells.
box : numpy.ndarray
	The sizes of the box edges.
rc : float
	The smallest size of the cells.
)pbdoc");

	cells.def("get_cell", &CellLists::get_cell, py::arg("pos"), R"pbdoc(
Return the cell corresponding to the given position. 

Parameters
----------
pos : numpy.ndarray
	The position whose corresponding cell should be returned.

Returns
-------
np.ndarray
    The three integer coordinates of the cell corresponding to the given position.
)pbdoc");

	cells.def("is_overlap", &CellLists::is_overlap, py::arg("particles"), py::arg("pos"), py::arg("cutoff"), R"pbdoc(
Check whether the given position is closer than the given cutoff to any particle indexed in the cells.

Parameters
----------
particles : List(:class:`Particle`)
	The particles used to build the cells
pos : numpy.ndarray
	The position to be checked.
cutoff : float
	The cutoff to be used in the computation.

Returns
-------
bool
	True if there is an overlap, False otherwise.
)pbdoc");

}

#endif

} /* namespace ba */
