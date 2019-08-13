/*
 * CellLists.h
 *
 *  Created on: Aug 10, 2019
 *      Author: lorenzo
 */

#ifndef UTILS_CELLLISTS_H_
#define UTILS_CELLLISTS_H_

#include "../defs.h"
#include "../particles/Particle.h"

namespace ba {

/**
 * @brief A data structure to manage cell lists of particles in a cubic box.
 *
 * This is a low-level structure and hence for performance reasons some of the members are public and accessible from the outside.
 */
struct CellLists {
public:
	/**
	 * @brief Construct the object. This is the only place where the user can decide to also build the cell shifts.
	 *
	 * @param init_shifts if true, cell shifts will be initialised upon calling init_cells. False by default
	 */
	CellLists(bool init_shifts=false);
	virtual ~CellLists();

	/**
	 * @brief Number of cells along each box dimension.
	 */
	glm::ivec3 N_cells_side;

	/**
	 * @brief Each element i of this vector contains either -1 or the index of another particle that is in the same cell of i.
	 */
	std::vector<int> next;

	/**
	 * @brief Each element c of this vector contains either the index of a particle that is in cell c or -1 if c is empty.
	 */
	std::vector<int> heads;

	/**
	 * @brief Initialise the cell machinery for the given particles, box size and cell edge length
	 *
	 * All data structures are reset at the beginning, so that this method can be called from the same instance as many times as you want.
	 *
	 * @param particles all the particles to be
	 * @param box the length of the box's sides
	 * @param rc the minimum size of a cell's edge
	 */
	void init_cells(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box, double rc);

	/**
	 * @brief Returns the index of the cell associated to the given position.
	 *
	 * @param pos
	 * @return the index of the cell (as integer)
	 */
	int get_cell_index(const vec3 &pos) const;

	/**
	 * @brief Returns the the cell associated to the given position.
	 *
	 * @param pos
	 * @return the 3-dimensional index of the cell (as a 3D vector of integers)
	 */
	glm::ivec3 get_cell(const vec3 &pos) const;

	/**
	 * @brief Returns a vector of shifts for each order (that is, distance from the central cell) of cells.
	 *
	 * This method throws an exception if the instance was initialised with init_shift=false
	 *
	 * @return the list of shifts associated to each order of cells
	 */
	const std::vector<std::vector<glm::ivec3>> &cell_shifts() const;

private:
	vec3 _curr_box;
	bool _init_shifts;
	std::vector<std::vector<glm::ivec3>> _cell_shifts;
};

} /* namespace ba */

#endif /* UTILS_CELLLISTS_H_ */
