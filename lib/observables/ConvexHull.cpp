/*
 * ConvexHull.cpp
 *
 *  Created on: 13 gen 2020
 *      Author: lorenzo
 */

#include "ConvexHull.h"

#include <quickhull/QuickHull.hpp>

namespace ba {

ConvexHull::ConvexHull() :
				SystemObservable<std::vector<ConvexHullResult>>() {

}

ConvexHull::~ConvexHull() {

}

void ConvexHull::analyse_system(std::shared_ptr<System> system) {
	ConvexHullResult res;

	quickhull::QuickHull<double> qh;
	std::vector<quickhull::Vector3<double>> point_cloud;

	for(auto pos: system->positions()) {
		point_cloud.emplace_back(pos[0], pos[1], pos[2]);
	}

	// the second parameter parameter makes sure that the resulting mesh will have its triangles in CCW orientation
	auto hull = qh.getConvexHull(point_cloud, true, false);
	auto index_buffer = hull.getIndexBuffer();
	auto vertex_buffer = hull.getVertexBuffer();

	for(uint i = 0, j = 0; i < index_buffer.size(); i += 3, j++) {
		vec3 p1(vertex_buffer[index_buffer[i + 0]].x, vertex_buffer[index_buffer[i + 0]].y, vertex_buffer[index_buffer[i + 0]].z);
		vec3 p2(vertex_buffer[index_buffer[i + 1]].x, vertex_buffer[index_buffer[i + 1]].y, vertex_buffer[index_buffer[i + 1]].z);
		vec3 p3(vertex_buffer[index_buffer[i + 2]].x, vertex_buffer[index_buffer[i + 2]].y, vertex_buffer[index_buffer[i + 2]].z);

		vec3 t_normal = glm::cross(p1 - p3, p2 - p3);

		res.volume += (glm::dot(p1, (glm::cross(p2, p3)))) / 6.;
		res.area += std::sqrt(glm::dot(t_normal, t_normal)) / 2.;
	}

	// the definition of volume we used above makes sure that all triangles are oriented either towards the outside or towards the inside, in which case we would obtain a negative volume
	if(res.volume < 0.) {
		res.volume *= -1.;
	}

	_result.push_back(res);
}

#ifdef PYTHON_BINDINGS

void export_ConvexHull(py::module &m) {
	py::class_<ConvexHull, std::shared_ptr<ConvexHull>> obs(m, "ConvexHull");

	obs
		.def(py::init<>());

	PY_EXPORT_SYSTEM_OBS(obs, ConvexHull);

	py::class_<ConvexHullResult, std::shared_ptr<ConvexHullResult>> res(m, "ConvexHullResult");

	res
		.def(py::init<>())
		.def_readwrite("volume", &ConvexHullResult::volume)
		.def_readwrite("area", &ConvexHullResult::area);
}

#endif

} /* namespace ba */
