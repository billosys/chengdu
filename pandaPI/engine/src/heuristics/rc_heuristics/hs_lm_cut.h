/*
 * hs_lm_cut.h
 *
 *  Created on: 13.12.2018
 *      Author: Daniel Höller
 */

#ifndef HEURISTICS_HSLMCUT_H_
#define HEURISTICS_HSLMCUT_H_

#include <cassert>
#include <climits>
#include <list>
#include "../../int_data_structures/int_pair_heap.h"
#include "../../int_data_structures/bucket_set.h"
#include "../../int_data_structures/no_del_int_set.h"
#include "../../int_data_structures/flex_int_stack.h"
#include "../../model.h"
#include "lm_cut_landmark.h"

namespace progression {

/*
class ComparePair {
public:
	bool operator()(pair<int, int>* n1, pair<int, int>* n2);
};*/

class hsLmCut {
public:
	hsLmCut(Model* sas);
	virtual ~hsLmCut();

	int getHeuristicValue(bucketSet& s, noDelIntSet& g);
	Model* m;
	
	string getDescription(){ return "lmc";}

	//list<LMCutLandmark*> cuts;
    list<LMCutLandmark *>* cuts = new list<LMCutLandmark *>();

private:
	IntUtil iu;
	int getHMax(bucketSet& s, noDelIntSet& g);
	int updateHMax(noDelIntSet& g, bucketSet* cut);
	void calcGoalZone(noDelIntSet* goalZone, bucketSet* cut, bucketSet* precsOfCutNodes);
	void forwardReachabilityDFS(bucketSet& s0, bucketSet* cut, noDelIntSet* goalZone, bucketSet* testReachability);

	int* costs;
	noDelIntSet* goalZone;
	bucketSet* cut;
	bucketSet* precsOfCutNodes;

	const bool storeCuts = true;

	// hMax stuff
	IntPairHeap<int>* heap;
	int* hValInit;

	int* unsatPrecs;
	int* maxPrecInit;
	int* hVal;
	FlexIntStack stack;
	int maxPrecG = -1;
    int* maxPrec;

	int* numAddToTask;
	int** addToTask;
	noDelIntSet* remove;
	noDelIntSet* visited;

};

} /* namespace progression */

#endif /* HEURISTICS_HSLMCUT_H_ */
