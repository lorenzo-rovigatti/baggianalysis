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

	_result.vertices.clear();
	_result.triangles.clear();
	double volume = 0.;
	double area = 0.;

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
		vec3 &p3 = _result.vertices[index_buffer[i + 2]];

		vec3 t_normal = glm::cross(p1 - p3, p2 - p3);
		ConvexHullTriangle nt(p1, p2, p3, t_normal);

		volume += nt.volume();
		area += nt.area();

		_result.triangles.emplace_back(nt);
	}

	// the definition of volume we used above makes sure that all triangles are oriented either towards the outside or towards the inside.
	// In the latter case we flip all the normals, as well as the sign of the resulting volume
	if(volume < 0.) {
		volume *= -1.;
		for(auto &triangle : _result.triangles) {
			triangle.normal = -triangle.normal;
		}
	}

	if(_average_over_trajectory) {
		_result.area += area;
		_result.volume += volume;
		_N_frames++;
	}
	else {
		_result.area = area;
		_result.volume = volume;
	}
}

void ConvexHull::analyse_trajectory(std::shared_ptr<BaseTrajectory> trajectory) {
	_average_over_trajectory = true;

	_N_frames = 0;
	reset();
	SystemObservable::analyse_trajectory(trajectory);
	_result.triangles.clear();
	_result.vertices.clear();
	_result.volume /= _N_frames;
	_result.area /= _N_frames;

	_average_over_trajectory = false;
}

#ifdef PYTHON_BINDINGS

void export_ConvexHull(py::module &m) {
	py::class_<ConvexHull, std::shared_ptr<ConvexHull>> obs(m, "ConvexHull", R"pbdoc(
        Construct the `convex hull <https://en.wikipedia.org/wiki/Convex_hull>`_ of the system, which is the smallest convex set that contains the set of points constituted by the particles' coordinates of the given system.
	)pbdoc");

	obs.def(py::init<>());

	PY_EXPORT_SYSTEM_OBS(obs, ConvexHull);

	py::class_<ConvexHullResult, std::shared_ptr<ConvexHullResult>> res(m, "ConvexHullResult", R"pbdoc(
        The actual convex hull.
	)pbdoc");
	res.def(py::init<>());
	res.def_readonly("volume", &ConvexHullResult::volume, R"pbdoc(
		The volume of the convex hull.
	)pbdoc");
	res.def_readonly("area", &ConvexHullResult::area, R"pbdoc(
			The area of the convex hull.
		)pbdoc");
	res.def_readonly("triangles", &ConvexHullResult::triangles, R"pbdoc(
		A list of :class:`Triangles<ConvexHullTriangle>` that compose the convex hull.
	)pbdoc");
	res.def_readonly("vertices", &ConvexHullResult::vertices, R"pbdoc(
		The vertices that compose the convex hull. These are a subset of the particles of the system.
	)pbdoc");

	py::class_<ConvexHullTriangle, std::shared_ptr<ConvexHullTriangle>> triangle(m, "ConvexHullTriangle", R"pbdoc(
        A triangle of the convex hull mesh. It contains its three vertices as well as its normal (*i.e.* the normal unit vector pointing outwards).
	)pbdoc");
	triangle.def_readonly("v1", &ConvexHullTriangle::v1, R"pbdoc(
		The first vertex.
	)pbdoc");
	triangle.def_readonly("v2", &ConvexHullTriangle::v2, R"pbdoc(
		The second vertex along the counter-clock-wise contour of the triangle.
	)pbdoc");
	triangle.def_readonly("v3", &ConvexHullTriangle::v3, R"pbdoc(
		The second vertex along the counter-clock-wise contour of the triangle.
	)pbdoc");
	triangle.def_readonly("normal", &ConvexHullTriangle::normal, R"pbdoc(
		The unit vector orthogonal to the triangle surface which points towards the exterior of convex hull.
	)pbdoc");
	triangle.def("area", &ConvexHullTriangle::area, R"pbdoc(
		Return the area of the triangle.
	)pbdoc");
	triangle.def("volume", &ConvexHullTriangle::volume, R"pbdoc(
		Return the volume of the triangle.
	)pbdoc");
}

#endif

} /* namespace ba */
