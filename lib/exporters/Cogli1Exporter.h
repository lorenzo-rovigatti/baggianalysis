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

class Particle;

struct Cogli1Particle {
	bool show = true;
	double size = 0.5;
	std::string color = "red";
};

using Cogli1Mapper = std::function<Cogli1Particle(Particle *)>;

class Cogli1Exporter: public BaseExporter {
public:
	Cogli1Exporter();
	Cogli1Exporter(Cogli1Mapper);
	virtual ~Cogli1Exporter();

private:
	void _write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) override;

	Cogli1Mapper _mapper = [](Particle *p) {
		return Cogli1Particle();
	};
};

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_COGLI1EXPORTER_H_ */
