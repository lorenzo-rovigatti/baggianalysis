/*
 * MSD.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_MSD_H_
#define OBSERVABLES_MSD_H_

#include "../Trajectory.h"

namespace ba {

struct MSDOptions {
	uint points_per_cycle = 0;
	std::string output_file = "msd.dat";
};

class MSD {
public:
	MSD(std::shared_ptr<Trajectory> trajectory);
	virtual ~MSD();

	static MSDOptions default_options() {
		return MSDOptions();
	}

	void compute_and_print(const MSDOptions &opts);

protected:
	double _conf_conf_MSD(std::shared_ptr<System> first, std::shared_ptr<System> second);

protected:
	std::shared_ptr<Trajectory> _trajectory;
};

} /* namespace ba */

#endif /* OBSERVABLES_MSD_H_ */
