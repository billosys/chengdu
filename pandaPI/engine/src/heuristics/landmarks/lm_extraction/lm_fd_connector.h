/*
 * lm_fd_connector.h
 *
 *  Created on: 09.02.2020
 *      Author: dh
 */

#ifndef HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_
#define HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_

#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "../../../model.h"
#include "../../../int_data_structures/string_util.h"
#include "../../rc_heuristics/rc_model_factory.h"

namespace progression {

class LmFdConnector {
public:
	LmFdConnector();
	virtual ~LmFdConnector();

	void createLMs(Model* htn);

	int numLMs = -1;
	int numConjunctive = -1;
	landmark** landmarks = nullptr;

	int getNumLMs();
	landmark** getLMs();

private:
	StringUtil su;

	void readFDLMs(string f, RCModelFactory* factory);
	int getIndex(string f, Model* rc);
};

} /* namespace progression */

#endif /* HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_ */
