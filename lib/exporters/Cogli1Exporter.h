/*
 * Cogli1Exporter.h
 *
 *  Created on: 5 mar 2020
 *      Author: lorenzo
 */

#ifndef EXPORTERS_COGLI1EXPORTER_H_
#define EXPORTERS_COGLI1EXPORTER_H_

#include "BaseExporter.h"

namespace ba {

using Cogli1Mapper = std::function<std::string(Particle *)>;

class Cogli1Exporter: public BaseExporter {
public:
	Cogli1Exporter();
	Cogli1Exporter(Cogli1Mapper);
	virtual ~Cogli1Exporter();

	void write(std::shared_ptr<System> system, std::string filename) override;

private:
	Cogli1Mapper _mapper = [](Particle *p) {
		return boost::str(boost::format("%lf %lf %lf @ 0.5 C[red]") % p->position()[0] % p->position()[1] % p->position()[2]);
	};
};

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_COGLI1EXPORTER_H_ */
