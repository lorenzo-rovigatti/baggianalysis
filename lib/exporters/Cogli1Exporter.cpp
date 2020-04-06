/*
 * Cogli1Exporter.cpp
 *
 *  Created on: 5 mar 2020
 *      Author: lorenzo
 */

#include "Cogli1Exporter.h"

#include "../utils/strings.h"

#include <fstream>

namespace ba {

Cogli1Exporter::Cogli1Exporter() {

}

Cogli1Exporter::Cogli1Exporter(Cogli1Mapper mapper) : _mapper(mapper) {

}

Cogli1Exporter::~Cogli1Exporter() {

}

void Cogli1Exporter::write(std::shared_ptr<System> system, std::string filename) {
	std::ofstream output_file(filename);

	output_file << boost::format(".Box:%lf,%lf,%lf") % system->box[0] % system->box[1] % system->box[2] << std::endl;

	for(auto p : system->particles()) {
		auto c_p = _mapper(p.get());
		if(c_p.show) {
			std::string p_string = boost::str(boost::format("%lf %lf %lf @ %lf C[%s]") % p->position()[0] % p->position()[1] % p->position()[2] % c_p.size % c_p.color);
			output_file << p_string << std::endl;
		}
	}

	output_file.close();
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
This constructor makes the exporter print each particle as a red sphere of radius 0.5.
	)pbdoc");

	// docstrings of overloaded constructors need to be unindented or they won't be formatted correctly by sphinx
	exporter.def(py::init<Cogli1Mapper>(), R"pbdoc(
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
mapper : callable
    A callable that takes a particle and returns a :class:`Cogli1Particle`.
	)pbdoc");
}

#endif

} /* namespace ba */
