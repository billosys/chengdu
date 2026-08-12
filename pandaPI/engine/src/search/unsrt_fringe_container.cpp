/*
 * unsrt_fringe_container.cpp
 *
 *  Created on: 18.12.2018
 *      Author: dh
 */

#include "unsrt_fringe_container.h"

namespace progression {

UnsrtFringeContainer::UnsrtFringeContainer(int id) {
	content = new searchNode*[containerSize];
	this->containerID = id;

}

UnsrtFringeContainer::~UnsrtFringeContainer() {
	delete[] content;
}

} /* namespace progression */
