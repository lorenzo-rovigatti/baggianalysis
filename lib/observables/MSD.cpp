/*
 * MSD.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "MSD.h"

#include <fstream>
#include <map>

using namespace std;

namespace ba {

MSD::MSD(std::shared_ptr<BaseTrajectory> trajectory) :
				_trajectory(trajectory) {

}

MSD::~MSD() {

}

void MSD::compute_and_print(const MSDOptions &opts) {
	if(opts.points_per_cycle < 1) {
		string error = boost::str(boost::format("The MSD's points per cycle (%d) should larger than 0") % opts.points_per_cycle);
		throw std::runtime_error(error);
	}

	uint ppc = opts.points_per_cycle;
	map<ullint, double> MSD;
	map<ullint, uint> n_conf;

	shared_ptr<System> current_cycle_base;
	vector<shared_ptr<System>> past_cycle_bases;

	uint N_first_conf = 0;
	uint idx = 0;


	auto frame = _trajectory->next_frame();
	while(frame != nullptr) {
		uint N_conf = frame->particles.N();
		if(N_first_conf == 0) {
			N_first_conf = N_conf;
			BOOST_LOG_TRIVIAL(info) << "Computing the MSD on configurations composed of " << N_conf << " particles";
		}

		if(N_conf != N_first_conf) {
			string error = boost::str(boost::format("The configurations at times 0 and %u contain different number of particles (%d != %d)") % frame->time % N_first_conf % N_conf);
			throw std::runtime_error(error);
		}

		if(idx % ppc == 0) {
			if(idx > 0) {
				// save the old cycle base
				past_cycle_bases.push_back(current_cycle_base);
			}

			current_cycle_base = frame;

			// loop over configurations that were the bases of past cycles
			for(auto past_base : past_cycle_bases) {
				double cc_MSD = _conf_conf_MSD(current_cycle_base, past_base, opts.remove_com);
				ullint time_diff = current_cycle_base->time - past_base->time;
				auto it = MSD.find(time_diff);
				if(it == MSD.end()) {
					MSD[time_diff] = cc_MSD;
					n_conf[time_diff] = 1;
				}
				else {
					MSD[time_diff] += cc_MSD;
					n_conf[time_diff]++;
				}
			}
		}

		double cc_MSD = _conf_conf_MSD(current_cycle_base, frame, opts.remove_com);
		ullint time_diff = frame->time - current_cycle_base->time;
		auto it = MSD.find(time_diff);
		if(it == MSD.end()) {
			MSD[time_diff] = cc_MSD;
			n_conf[time_diff] = 1;
		}
		else {
			MSD[time_diff] += cc_MSD;
			n_conf[time_diff]++;
		}

		idx++;
		frame = _trajectory->next_frame();
	}

	// print the MSD
	ofstream output_file(opts.output_file);

	for(auto pair : MSD) {
		if(pair.first > 0) {
			double value = pair.second / n_conf[pair.first];
			output_file << pair.first << " " << value << endl;
		}
	}

	output_file.close();
}

double MSD::_conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second, bool remove_com) {
	double cc_MSD = 0.;
	uint N = first->particles.N();
	vec3 com_diff(0., 0., 0.);
	if(remove_com) {
		com_diff = second->com() - first->com();
	}
	for(uint i = 0; i < N; i++) {
		vec3 diff = second->particles.positions[i] - first->particles.positions[i] - com_diff;
		cc_MSD += diff.dot(diff);
	}

	return cc_MSD / N;
}

} /* namespace ba */
