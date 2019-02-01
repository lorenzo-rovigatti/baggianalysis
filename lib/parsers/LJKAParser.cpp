/*
 * LJKAParser.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "LJKAParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

using namespace std;

LJKAParser::LJKAParser() :
				BaseParser() {

}

LJKAParser::~LJKAParser() {

}

shared_ptr<System> LJKAParser::parse(ifstream &configuration) {
	string line;
	vector<string> split;
	uint N, NA;

	// line containing the timestep and the number of particles
	getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(new System);

	boost::split(split, line, boost::is_any_of(" "));
	try {
		syst->time = boost::lexical_cast<ullint>(boost::trim_copy(split[0]));
		N = boost::lexical_cast<uint>(boost::trim_copy(split[2]));
		NA = boost::lexical_cast<uint>(boost::trim_copy(split[3]));
	}
	catch(boost::bad_lexical_cast &e) {
		string error = boost::str(boost::format("The timestep value '%s' found in the oxDNA configuration cannot be cast to an integer") % split[2]);
		throw std::runtime_error(error);
	}

	// box line
	getline(configuration, line);
	string to_split = boost::trim_copy(line);
	boost::split(split, to_split, boost::is_any_of(" "));
	try {
		syst->box[0] = boost::lexical_cast<double>(boost::trim_copy(split[0]));
		syst->box[1] = boost::lexical_cast<double>(boost::trim_copy(split[1]));
		syst->box[2] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
	}
	catch(boost::bad_lexical_cast &e) {
		string error = boost::str(boost::format("The box line '%s' found in the LJKA configuration is not valid") % line);
		throw std::runtime_error(error);
	}

	for(uint i = 0; i < N; i++) {
		getline(configuration, line);
		boost::split(split, line, boost::is_any_of(" "));
		vec3 position = vec3(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]), boost::lexical_cast<double>(split[2]));

		particle_type type = (i < NA) ? 0 : 1;
		syst->particles.types.push_back(type);
		syst->particles.positions.push_back(position);
		syst->particles.velocities.push_back(vec3(0., 0., 0.));
	}

	return syst;
}

} /* namespace ba */
