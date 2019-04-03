/*
 * GroParser.cpp
 *
 *  Created on: 31 gen 2019
 *      Author: lorenzo
 */

#include "GroParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

using namespace std;

GroParser::GroParser(double dt) :
				BaseParser(),
				_dt(dt) {

}

GroParser::~GroParser() {

}

shared_ptr<System> GroParser::parse(ifstream &configuration) {
	string line;
	vector<string> split;

	// timestep line
	getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(new System);

	auto time_pos = line.find("t=");
	if(time_pos == string::npos) {
		string error = boost::str(boost::format("Malformed first line '%s' (cannot find 't=')") % line);
		throw std::runtime_error(error);
	}

	try {
		string time_string = boost::trim_copy(line.substr(time_pos + 2));
		double time_double = boost::lexical_cast<double>(time_string);
		syst->time = std::round(time_double / _dt);
	}
	catch(boost::bad_lexical_cast &e) {
		string error = boost::str(boost::format("The timestep '%s' found in the .gro configuration cannot be cast to a double") % split[2]);
		throw std::runtime_error(error);
	}

	// number of particles line
	getline(configuration, line);
	uint N;
	try {
		N =  boost::lexical_cast<uint>(boost::trim_copy(line));
	}
	catch(boost::bad_lexical_cast &e) {
		string error = boost::str(boost::format("The number of particles '%s' found in the .gro configuration cannot be cast to an integer") % split[2]);
		throw std::runtime_error(error);
	}

	vector<string> inserted_name;
	for(uint i = 0; i < N; i++) {
		getline(configuration, line);

		// taken by http://manual.gromacs.org/archive/5.0.3/online/gro.html
		string residue_number = line.substr(0, 5);
		string residue_name = line.substr(5, 5);
		string atom_name = line.substr(10, 5);
		string atom_number = line.substr(15, 5);

		boost::trim(atom_name);
		auto it = find(inserted_name.begin(), inserted_name.end(), atom_name);
		particle_type p_type;
		if(it == inserted_name.end()) {
			p_type = inserted_name.size();
			inserted_name.push_back(atom_name);
		}
		else {
			p_type = std::distance(inserted_name.begin(), it);
		}

		syst->particles.types().push_back(p_type);

		string x = boost::trim_copy(line.substr(20, 8));
		string y = boost::trim_copy(line.substr(28, 8));
		string z = boost::trim_copy(line.substr(36, 8));

		try {
			syst->particles.positions().push_back(vec3(boost::lexical_cast<double>(x), boost::lexical_cast<double>(y), boost::lexical_cast<double>(z)));
		}
		catch(boost::bad_lexical_cast &e) {
			string error = boost::str(boost::format("The position of the %u-th particle (%s, %s, %s) cannot be cast to a vector of floating-point numbers") % i % x % y % z);
			throw std::runtime_error(error);
		}

		string vx = boost::trim_copy(line.substr(44, 8));
		string vy = boost::trim_copy(line.substr(52, 8));
		string vz = boost::trim_copy(line.substr(60, 8));

		try {
			syst->particles.velocities().push_back(vec3(boost::lexical_cast<double>(vx), boost::lexical_cast<double>(vy), boost::lexical_cast<double>(vz)));
		}
		catch(boost::bad_lexical_cast &e) {
			string error = boost::str(boost::format("The velocity of the %u-th particle (%s, %s, %s) cannot be cast to a vector of floating-point numbers") % i % vx % vy % vz);
			throw std::runtime_error(error);
		}
	}

	// box line
	getline(configuration, line);
	string to_split = boost::trim_copy(line);
	boost::split(split, to_split, boost::is_any_of(" "), boost::algorithm::token_compress_on);
	try {
		syst->box[0] = boost::lexical_cast<double>(boost::trim_copy(split[0]));
		syst->box[1] = boost::lexical_cast<double>(boost::trim_copy(split[1]));
		syst->box[2] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
	}
	catch(boost::bad_lexical_cast &e) {
		string error = boost::str(boost::format("The box line '%s' found in the .gro configuration is not valid") % line);
		throw std::runtime_error(error);
	}

	return syst;
}

} /* namespace ba */
