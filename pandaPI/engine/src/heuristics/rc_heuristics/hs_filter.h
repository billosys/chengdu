/*
 * hs_filter.h
 *
 *  Created on: 19.12.2018
 *      Author: dh
 */

#ifndef HEURISTICS_HSFILTER_H_
#define HEURISTICS_HSFILTER_H_

#include "../../int_data_structures/int_pair_heap.h"
#include "../../int_data_structures/bucket_set.h"
#include "../../int_data_structures/no_del_int_set.h"
#include "../../int_data_structures/int_stack.h"
#include "hs_add_ff.h"
#include "lm_cut_landmark.h"

namespace progression {

class hsFilter {
public:
	hsFilter(Model* sas);
	virtual ~hsFilter();
		
	string getDescription(){ return "filter";}

    list<LMCutLandmark *>* cuts = new list<LMCutLandmark *>();
	
	int getHeuristicValue(bucketSet& s, noDelIntSet& g);
	Model* m;
private:
	hsAddFF* add;
};
} /* namespace progression */

#endif /* HEURISTICS_HSFILTER_H_ */
