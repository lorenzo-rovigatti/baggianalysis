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
	if(argc < 6) {
		std::cerr << "Usage is " << argv[0] << " pattern ppc output species=-1|0|1|(-1 means all the particles) only_com=0|1" << std::endl;
		return 1;
	}

	std::shared_ptr<ba::BaseParser> parser(new ba::GroParser(0.001));
	std::shared_ptr<ba::LazyTrajectory> trajectory(new ba::LazyTrajectory(parser));

	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FixParticlePath()));
	trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::SubtractCOM()));

	int species = atoi(argv[4]);
	if(species != -1) {
		std::vector<particle_type> allowed_types = {(uint) species};
		trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FilterByType(allowed_types)));
	}

	int only_com = atoi(argv[5]);
	if(only_com) {
		trajectory->add_filter(std::shared_ptr<ba::BaseFilter>(new ba::FilterByReducingToCOM()));
	}

	trajectory->initialise_from_folder(".", argv[1]);

	ba::MSD msd(trajectory);

	auto opts = ba::MSD::default_options();
	opts.points_per_cycle = atoi(argv[2]);
	opts.remove_com = false;
	opts.output_file = std::string(argv[3]);
	msd.compute_and_print(opts);

	return 0;
}