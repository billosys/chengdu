/*
 * unsorted_fringe.h
 *
 *  Created on: 18.12.2018
 *      Author: dh
 */

#ifndef UNSORTEDFRINGE_H_
#define UNSORTEDFRINGE_H_

#include "../progression_network.h"
#include "unsrt_fringe_container.h"

namespace progression {

class UnsortedFringe {
public:
	UnsortedFringe();
	virtual ~UnsortedFringe();

	void addFirst(searchNode* n);
	void addLast(searchNode* n);
	bool empty();
	searchNode* removeLast();
	searchNode* removeFirst();
	int size();

private:
	UnsrtFringeContainer* firstContainer = nullptr;
	UnsrtFringeContainer* lastContainer = nullptr;
	int sizeCounter = 0;

	int currentID = 0;
};

} /* namespace progression */

#endif /* UNSORTEDFRINGE_H_ */
