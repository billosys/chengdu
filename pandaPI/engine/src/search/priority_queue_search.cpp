/*
 * priority_queue_search.cpp
 *
 *  Created on: 10.09.2017
 *      Author: Daniel Höller
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cassert>
#include <chrono>
#include <fstream>

#include "priority_queue_search.h"
#include "../progression_network.h"
#include "../model.h"
#include "../int_data_structures/flex_int_stack.h"


#ifdef PREFMOD
// preferred modifications
#include "AlternatingFringe.h"
#endif

#include <queue>
#include <map>
#include <algorithm>
#include <bitset>

namespace progression {

PriorityQueueSearch::PriorityQueueSearch() {
	// TODO Auto-generated constructor stub
}

PriorityQueueSearch::~PriorityQueueSearch() {
	// TODO Auto-generated destructor stub
}

searchNode* PriorityQueueSearch::handleNewSolution(searchNode* newSol, searchNode* oldSol, long time) {
	searchNode* res;
	foundSols++;
	if(oldSol == nullptr) {
		res = newSol;
		firstSolTime = time;
		bestSolTime = time;
		if (this->optimzeSol) {
			cout << "SOLUTION: (" << time << "ms) Found first solution with action costs of " << newSol->actionCosts << "." << endl;
		}
	} else if(newSol->actionCosts < oldSol->actionCosts) {
		// shall optimize until time limit, this is a better one
		bestSolTime = time;
		res = newSol;
		solImproved++;
		cout << "SOLUTION: (" << time << "ms) Found new solution with action costs of " << newSol->actionCosts << "." << endl;
	} else {
		//cout << "Found new solution with action costs of " << newSol->actionCosts << "." << endl;
		res = oldSol;
	}
	return res;
}

/// closing namespace
}
