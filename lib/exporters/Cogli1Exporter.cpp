/*
 * Cogli1Exporter.cpp
 *
 *  Created on: 5 mar 2020
 *      Author: lorenzo
 */

#include "Cogli1Exporter.h"

#include "../System.h"
#include "../utils/strings.h"

#include <fstream>

namespace ba {

Cogli1Exporter::Cogli1Exporter() : Cogli1Exporter([](Particle *p) {return Cogli1Particle();}) {

}

Cogli1Exporter::Cogli1Exporter(Cogli1Mapper mapper) {
	_converter = [mapper](Particle *p) {
		Cogli1Particle c_p = mapper(p);
		if(c_p.show) {
			return fmt::format("{} {} {} @ {} C[{}]", p->position()[0], p->position()[1], p->position()[2], c_p.size, c_p.color);
		}
		else {
			return std::string();
		}
	};
}

Cogli1Exporter::Cogli1Exporter(Cogli1Converter converter, bool) : _converter(converter) {

}

Cogli1Exporter::~Cogli1Exporter() {

}

void Cogli1Exporter::_write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) {
	output << fmt::format(".Box:{},{},{}", system->box[0], system->box[1], system->box[2]) << std::endl;

	for(auto p : system->particles()) {
		std::string p_string = _converter(p.get());
		if(p_string.size() > 0) {
			output << p_string << std::endl;
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m) {
	py::class_<Cogli1Particle> particle(m, "Cogli1Particle", R"pbdoc(
        A utility class used by :class:`Cogli1Exporter` to customise its output.
	)pbdoc");

	particle.def(py::init<>(), R"pbdoc(
		By default, the particle is visible, has a size of 0.5 and a red color.
	)pbdoc");

	particle.def_readwrite("show", &Cogli1Particle::show, R"pbdoc(
        bool : If true, the particle will be printed by the exporter. Defaults to True.
	)pbdoc");

	particle.def_readwrite("size", &Cogli1Particle::size, R"pbdoc(
		float : The size of the particle. For spheres this corresponds to the radius. Defaults to 0.5.
	)pbdoc");

	particle.def_readwrite("color", &Cogli1Particle::color, R"pbdoc(
		str : The color of the particle. It can be a name ("red") or an RGB string ("1,0,0"). Defaults to "red".
	)pbdoc");

	py::class_<Cogli1Exporter, BaseExporter, std::shared_ptr<Cogli1Exporter>> exporter(m, "Cogli1Exporter", R"pbdoc(
		Export configurations to the cogli1 file format, used for visualisation purposes.
	)pbdoc");

	exporter.def(py::init<>(), R"pbdoc(
The default constructor makes the exporter print each particle as a red sphere of radius 0.5.
	)pbdoc");

	// docstrings of overloaded constructors need to be unindented or they won't be formatted correctly by sphinx
	exporter.def(py::init<Cogli1Mapper>(), py::arg("mapper"), R"pbdoc(
This constructor takes as a parameter a callable that maps each particle into a :class:`Cogli1Particle` that will be 
interpreted by the exporter and printed in cogli1 format.

An example of such a callable is the following::

    def cogli1_mapper(p):
        c_p = ba.Cogli1Particle()
        if p.position[0] > 0:
            c_p.size = 1.0
            c_p.color = "0,1,0"
        if p.position[1] > 0:
            c_p.show = False
        
        return c_p

The function defined above will give all those particles that have an x coordinate larger than 0 a size of 1.0 and a green color. 
Particles with a y coordinate larger than 0 will be hidden. The other particles will be red and have a size of 0.5.  

Parameters
----------
mapper: callable
    A callable that takes a particle and returns a :class:`Cogli1Particle`.
	)pbdoc");

	exporter.def(py::init<Cogli1Converter, bool>(), py::arg("converter"), py::arg("dummy"), R"pbdoc(
This constructor takes two parameters: a callable that maps each particle into a string and a dummy
boolean required to make the c++->Python conversion work.
It can be used to fully customise the output. The following example uses such flexibility to use cogli1 to visualise patchy particles::

    delta = 0.24
    cosmax = 0.98
    theta = math.acos(cosmax)
    def converter(p):
        ret = "%lf %lf %lf @ 0.5 C[red] M " % (p.position[0], p.position[1], p.position[2])

        for patch in p.orientation_vectors:
            patch_pos = patch * (0.5 + delta)
            ret += "%lf %lf %lf %lf C[blue] " % (patch_pos[0], patch_pos[1], patch_pos[2], theta)

        return ret

The function defined above will show the patches associated to the particles as blue cones of the given length and width in cogli1.

Parameters
----------
converter: callable
	A callable that takes a particle and returns a string.
dummy: bool
    A dummy boolean. Its value does not affect the behaviour of the exporter.
	)pbdoc");
}

#endif

} /* namespace ba */
