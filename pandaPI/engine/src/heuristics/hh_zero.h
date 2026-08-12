/*
 * HtnhZero.h
 *
 *  Created on: 16.09.2017
 *      Author: Daniel Höller
 */

#ifndef HTNHZERO_H_
#define HTNHZERO_H_

#include "../model.h"
#include "heuristic.h"

namespace progression {

class hhZero : public Heuristic {
public:
	hhZero(Model* htn, int index);
	virtual ~hhZero();
	string getDescription(){ return "zero()";}
	void setHeuristicValue(searchNode *n, searchNode *parent, int action);
	void setHeuristicValue(searchNode *n, searchNode *parent, int absTask, int method);
};

} /* namespace progression */

#endif /* HTNHZERO_H_ */
