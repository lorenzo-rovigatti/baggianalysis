/*
 * BaseTrajectory.cpp
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#include "BaseTrajectory.h"

namespace ba {

using namespace std;

BaseTrajectory::BaseTrajectory(shared_ptr<BaseParser> parser) :
				_parser(parser) {

}

BaseTrajectory::~BaseTrajectory() {

}

void BaseTrajectory::add_filter(shared_ptr<BaseFilter> filter) {
	_filters.push_back(filter);
}

} /* namespace ba */
