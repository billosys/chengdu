#ifndef TRANSCONTROLLER_H_
#define TRANSCONTROLLER_H_

#include "../model.h"


enum TranslationType{
	ParallelSeq, Push, TO, BaseStrips, BaseCondEffects
};


int runTranslationPlanner(Model* htn, TranslationType transtype, bool forceTransType,
		int pgb, int pgbsteps, string downward, string downwardConf, string sasfile,
		bool iterate,
		bool onlyGenerate,
		bool realCosts);

#endif
