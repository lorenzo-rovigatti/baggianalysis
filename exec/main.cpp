/*
 * main.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include <Trajectory.h>

#include <filters/FilterByReducingToCOM.h>
#include <filters/FilterByType.h>
#include <filters/SubtractCOM.h>
#include <observables/MSD.h>
#include <parsers/OxDNAParser.h>

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " topology trajectory" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::BaseParser> parser(new ba::OxDNAParser(argv[1]));

	std::shared_ptr<ba::Trajectory> trajectory(new ba::Trajectory(parser));

//	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FilterByReducingToCOM));
//	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::SubtractCOM));

	trajectory->initialise_from_trajectory_file(argv[2]);
//	trajectory->initialise_from_folder(argv[2], "conf_");

	ba::MSD msd(trajectory);

	auto opts = ba::MSD::default_options();
	opts.points_per_cycle = 20;
	opts.remove_com = true;
	msd.compute_and_print(opts);

	return 0;
}
