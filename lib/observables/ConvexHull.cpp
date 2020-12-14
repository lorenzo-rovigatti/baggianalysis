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
				SystemObservable<ConvexHullResult>() {

}

ConvexHull::~ConvexHull() {

}

void ConvexHull::analyse_system(std::shared_ptr<System> system) {
	quickhull::QuickHull<double> qh;
	std::vector<quickhull::Vector3<double>> point_cloud;
	_result = ConvexHullResult();

	for(auto pos: system->positions()) {
		point_cloud.emplace_back(pos[0], pos[1], pos[2]);
	}

	// the second parameter parameter makes sure that the resulting mesh will have its triangles in CCW orientation
	auto hull = qh.getConvexHull(point_cloud, true, false);
	auto index_buffer = hull.getIndexBuffer();
	auto vertex_buffer = hull.getVertexBuffer();

	// copy the vertices over to "our" data structure
	for(auto it = vertex_buffer.begin(); it != vertex_buffer.end(); it++) {
		vec3 v(it->x, it->y, it->z);
		_result.vertices.emplace_back(v);
	}

	for(uint i = 0, j = 0; i < index_buffer.size(); i += 3, j++) {
		vec3 &p1 = _result.vertices[index_buffer[i + 0]];
		vec3 &p2 = _result.vertices[index_buffer[i + 1]];
		vec3 &p3 =_result.vertices[index_buffer[i + 2]];

		vec3 t_normal = glm::cross(p1 - p3, p2 - p3);

		_result.volume += (glm::dot(p1, (glm::cross(p2, p3)))) / 6.;
		_result.area += std::sqrt(glm::dot(t_normal, t_normal)) / 2.;

		ConvexHullTriangle nt(p1, p2, p3, t_normal);
		_result.triangles.emplace_back(nt);
	}

	// the definition of volume we used above makes sure that all triangles are oriented either towards the outside or towards the inside, in which case we would obtain a negative volume
	if(_result.volume < 0.) {
		_result.volume *= -1.;
	}
}

#ifdef PYTHON_BINDINGS

void export_ConvexHull(py::module &m) {
	py::class_<ConvexHull, std::shared_ptr<ConvexHull>> obs(m, "ConvexHull");

	obs.def(py::init<>());

	PY_EXPORT_SYSTEM_OBS(obs, ConvexHull);

	py::class_<ConvexHullResult, std::shared_ptr<ConvexHullResult>> res(m, "ConvexHullResult");
	res.def(py::init<>());
	res.def_readwrite("volume", &ConvexHullResult::volume);
	res.def_readwrite("area", &ConvexHullResult::area);
	res.def_readwrite("triangles", &ConvexHullResult::triangles);
	res.def_readwrite("vertices", &ConvexHullResult::vertices);

	py::class_<ConvexHullTriangle, std::shared_ptr<ConvexHullTriangle>> triangle(m, "ConvexHullTriangle");
	triangle.def_readwrite("v1", &ConvexHullTriangle::v1);
	triangle.def_readwrite("v2", &ConvexHullTriangle::v2);
	triangle.def_readwrite("v3", &ConvexHullTriangle::v3);
	triangle.def_readwrite("normal", &ConvexHullTriangle::normal);
}

#endif

} /* namespace ba */
