/*
 * MSD.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "MSD.h"

#include <fstream>

using namespace std;

namespace ba {

MSD::MSD(shared_ptr<BaseTrajectory> trajectory) :
				_trajectory(trajectory) {

}

MSD::~MSD() {

}

std::map<ullint, double> MSD::compute(uint points_per_cycle, bool remove_com) {
	if(points_per_cycle < 1) {
		string error = boost::str(boost::format("The MSD's points per cycle (%d) should larger than 0") % points_per_cycle);
		throw std::runtime_error(error);
	}

	map<ullint, double> MSD;
	map<ullint, uint> n_conf;

	shared_ptr<System> current_cycle_base;
	vector<shared_ptr<System>> past_cycle_bases;

	uint N_first_conf = 0;
	uint idx = 0;

	auto frame = _trajectory->next_frame();
	while(frame != nullptr) {
		uint N_conf = frame->N();
		if(N_first_conf == 0) {
			N_first_conf = N_conf;
			BOOST_LOG_TRIVIAL(info) << "Computing the MSD on configurations composed of " << N_conf << " particles";
		}

		if(N_conf != N_first_conf) {
			string error = boost::str(boost::format("The configurations at times 0 and %u contain different number of particles (%d != %d)") % frame->time % N_first_conf % N_conf);
			throw std::runtime_error(error);
		}

		if(idx % points_per_cycle == 0) {
			if(idx > 0) {
				// save the old cycle base
				past_cycle_bases.push_back(current_cycle_base);
			}

			current_cycle_base = frame;

			// loop over configurations that were the bases of past cycles
			for(auto past_base : past_cycle_bases) {
				double cc_MSD = _conf_conf_MSD(current_cycle_base, past_base, remove_com);
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

		double cc_MSD = _conf_conf_MSD(current_cycle_base, frame, remove_com);
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

	for(auto &pair : MSD) {
		if(pair.first > 0) {
			pair.second /= n_conf[pair.first];
		}
	}

	return MSD;
}

void MSD::compute_and_print(uint points_per_cycle, bool remove_com, std::string output_file) {
	auto MSD = compute(points_per_cycle, remove_com);

	// print the MSD
	ofstream output(output_file);

	for(auto pair : MSD) {
		if(pair.first > 0) {
			output << pair.first << " " << pair.second << endl;
		}
	}

	output.close();
}

double MSD::_conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second, bool remove_com) {
	double cc_MSD = 0.;
	uint N = first->N();
	vec3 com_diff(0., 0., 0.);
	if(remove_com) {
		com_diff = second->com() - first->com();
	}
	for(uint i = 0; i < N; i++) {
		vec3 diff = second->particles()[i]->position() - first->particles()[i]->position() - com_diff;
		cc_MSD += glm::dot(diff, diff);
	}

	return cc_MSD / N;
}

#ifdef PYTHON_BINDINGS

void export_MSD(py::module &m) {
	py::class_<MSD, std::shared_ptr<MSD>> MSD(m, "MSD");

	MSD
		.def(py::init<shared_ptr<BaseTrajectory>>())
		.def("compute", &MSD::compute)
		.def("compute_and_print", &MSD::compute_and_print);
}

#endif

} /* namespace ba */
