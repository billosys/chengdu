/*
 * sAddFF.h
 *
 *  Created on: 23.09.2017
 *      Author: Daniel Höller
 */

#ifndef HEURISTICS_HSADDFF_H_
#define HEURISTICS_HSADDFF_H_

#include <climits>
#include <utility>
#include "../../int_data_structures/int_pair_heap.h"
#include "../../int_data_structures/bucket_set.h"
#include "../../int_data_structures/no_del_int_set.h"
#include "../../int_data_structures/int_stack.h"
#include "../../model.h"

using namespace std;

namespace progression {

    enum myHeu {
        sasAdd, sasFF
    };

    class ComparePair {
    public:
        bool operator()(pair<int, int> *n1, pair<int, int> *n2);
    };

    class hsAddFF {
    public:
        hsAddFF(Model *sas);

        virtual ~hsAddFF();
	
		string getDescription(){ return "ff";}

#if (STATEREP == SRCALC1) || (STATEREP == SRCOPY)

        int getHeuristicValue(bucketSet &s, noDelIntSet &g);

#elif (STATEREP == SRCALC2)
        int getHeuristicValue(noDelIntSet& s, noDelIntSet& g);
#elif(STATEREP == SRLIST)
        int getHeuristicValue(noDelIntSet& s, noDelIntSet& g);
#endif
        Model *m;
        myHeu heuristic = sasFF;
        int calls = 0;
    private:
        // todo: when parallelized, this must be per core
        IntPairHeap *queue;
        tHVal *hValPropInit;

        int *numSatPrecs;
        tHVal *hValOp;
        tHVal *hValProp;
        int *reachedBy;

        noDelIntSet markedFs;
        noDelIntSet markedOps;
        IntStack needToMark;

        bool allActionsCostOne = false;

        int getFF(noDelIntSet &g, int hVal);

        bool firstOverflow = true;
    };

} /* namespace progression */

#endif /* HEURISTICS_HSADDFF_H_ */
