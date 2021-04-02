/*
 * python_glm.h
 *
 *  Created on: Mar 28, 2019
 *      Author: lorenzo
 *
 * See https://stackoverflow.com/questions/42645228/cast-numpy-array-to-from-custom-c-matrix-class-using-pybind11
 */

#ifndef PYTHON_PYTHON_GLM_H_
#define PYTHON_PYTHON_GLM_H_

#include "../defs.h"

#include <pybind11/include/pybind11/numpy.h>
#include <glm/gtc/type_ptr.hpp>

namespace pybind11 {
namespace detail {

using std::cout;
using std::endl;

template<typename scalar, glm::precision P>
struct type_caster<glm::tvec3<scalar, P>> {
public:

	using vector_type = glm::tvec3<scalar, P>;

	PYBIND11_TYPE_CASTER(vector_type, _("glm::tvec3<scalar, P>"));

	// Python -> c++ conversion
	bool load(handle src, bool convert) {
		if(!convert and !array_t<scalar>::check_(src)) {
			return false;
		}

		auto buf = array_t<scalar, array::c_style | array::forcecast>::ensure(src);
		if(!buf) {
			return false;
		}

		if(buf.ndim() != 1) {
			return false;
		}

		if(buf.shape(0) != 3) {
			return false;
		}

		value = glm::make_vec3(buf.mutable_data());

		return true;
	}

	// c++ -> Python conversion
	static handle cast(const vec3 &src, return_value_policy /* policy */, handle /* parent */) {
		return array(3, glm::value_ptr(src)).release();
	}
};

template<typename scalar, glm::precision P>
struct type_caster<glm::tmat3x3<scalar, P>> {
	using matrix_type = glm::tmat3x3<scalar, P>;
	static constexpr std::size_t num_rows = 3;
	static constexpr std::size_t num_cols = 3;

	bool load(handle src, bool) {
		array_t<scalar> buf(src, true);
		if(!buf.check())
		return false;

		if(buf.ndim() == 2) { // a 2-dimensional matrix
			if(buf.shape(0) != num_rows || buf.shape(1) != num_cols) {
				return false; // not a 4x4 matrix
			}
			if(buf.strides(0) / sizeof(scalar) != num_cols || buf.strides(1) != sizeof(scalar)) {
				std::cout << "An array with non-standard strides is given. Please pass a contiguous array." << std::endl;
				return false;
			}
			// What we get from Python is laid out in row-major memory order, while GLM's
			// storage is col-major, thus, we transpose.
			value = glm::transpose(glm::make_mat4x4(buf.mutable_data())); // make_mat*() copies the data (unnecessarily)
		}
		else { // buf.ndim() != 2
			return false;
		}
		return true;
	}

	static handle cast(const matrix_type& src, return_value_policy /* policy */, handle /* parent */) {
		return array(
				{ num_rows, num_cols }, // shape
				{ sizeof(scalar), sizeof(scalar) * num_rows }, // strides - flip the row/col layout!
				glm::value_ptr(src)                            // data
						).release();
	}

	// Specifies the doc-string for the type in Python:
	PYBIND11_TYPE_CASTER(matrix_type, _("glm::tmat3x3<scalar, P>"));
};

}
/* namespace detail */
} /* namespace pybind11 */

#endif /* PYTHON_PYTHON_GLM_H_ */
