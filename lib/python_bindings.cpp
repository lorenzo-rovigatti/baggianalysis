/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "parsers/BaseParser.h"

#include "python_defs.h"

PYBIND11_MODULE(baggianalysis, m) {
	ba::export_BaseParser(m);
}
