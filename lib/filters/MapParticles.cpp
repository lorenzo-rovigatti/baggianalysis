/*
 * MapParticles.cpp
 *
 *  Created on: Jan 19, 2021
 *      Author: lorenzo
 */

#include "MapParticles.h"

namespace ba {

MapParticles::MapParticles(std::vector<std::vector<int>> id_lists) :
				_id_lists(id_lists) {

}

MapParticles::~MapParticles() {

}

std::shared_ptr<System> MapParticles::filter(std::shared_ptr<const System> syst) {
	std::shared_ptr<System> new_syst(syst->empty_copy());

	for(auto id_list : _id_lists) {
		vec3 com_pos(0., 0., 0.);
		vec3 com_vel(0., 0., 0.);
		for(auto particle_id : id_list) {
			try {
				auto particle = syst->particle_by_id(particle_id);
				com_pos += particle->position();
				com_vel += particle->velocity();
			}
			catch (std::exception &e) {
				std::string error = fmt::format("Caught the following error while applying a MapParticles filter:\n\t{}", e.what());
				throw std::runtime_error(error);
			}
		}
		com_pos /= id_list.size();
		com_vel /= id_list.size();

		int new_id = new_syst->available_index();
		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(new_id, DEFAULT_PARTICLE_TYPE, com_pos, com_vel));
		new_syst->add_particle(new_particle);
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_MapParticles(py::module &m) {
	py::class_<MapParticles, BaseFilter, std::shared_ptr<MapParticles>> filter(m, "MapParticles", R"pbdoc(
A filter that reduces the number of particles in a configuration by averaging the position and velocity of sets of particles.

As an example, the following snippet creates a filter that will include in the new system two particles whose position and
velocity will be computed by averaging the positions and velocities of particles with index 0 and 3 and 2, 5 and 6, respectively::

    id_lists = [[0, 3], [2, 5, 6]]
    my_filter = ba.MapParticles(id_lists)

	)pbdoc");

	filter.def(py::init<std::vector<std::vector<int>>>(), R"pbdoc(
        The constructor takes a list of index lists used to construct the new particles.

        Parameters
        ----------
        id_lists : List(List(int))
            A list of lists containing the indexes of the particles that should be averaged over to build the new particles.
    )pbdoc");
}

#endif

} /* namespace ba */
