/*
 * main.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include <filters/FilterByReducingToCOM.h>
#include <filters/FilterByType.h>
#include <filters/FixParticlePath.h>
#include <filters/SubtractCOM.h>
#include <observables/MSD.h>
#include <parsers/GroParser.h>
#include <parsers/LJKAParser.h>
#include "../lib/trajectories/FullTrajectory.h"
#include "../lib/trajectories/LazyTrajectory.h"

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cerr << "Usage is " << argv[0] << " folder ppc" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::BaseParser> parser(new ba::GroParser(0.001));
//	std::shared_ptr<ba::FullTrajectory> trajectory(new ba::FullTrajectory(parser));
	std::shared_ptr<ba::LazyTrajectory> trajectory(new ba::LazyTrajectory(parser));

	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FixParticlePath()));
	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::SubtractCOM()));
//	std::vector<particle_type> allowed_types = {0};
//	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FilterByType(allowed_types)));
//	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FilterByReducingToCOM()));

//	trajectory->initialise_from_trajectory_file(argv[2]);
	trajectory->initialise_from_folder(argv[1], "Cnf3500-*.gro");

	ba::MSD msd(trajectory);

	auto opts = ba::MSD::default_options();
	opts.points_per_cycle = atoi(argv[2]);
	opts.remove_com = false;
	opts.output_file = "msd_total.dat";
	msd.compute_and_print(opts);

	return 0;
}
