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

}
/* namespace detail */
} /* namespace pybind11 */

#endif /* PYTHON_PYTHON_GLM_H_ */
