/*
 * queue_fringe.cpp
 *
 *  Created on: 18.12.2018
 *      Author: Daniel Höller
 */

#include "queue_fringe.h"

namespace progression {

QueueFringe::QueueFringe() {
	f = new UnsortedFringe;
}

QueueFringe::~QueueFringe() {
	delete f;
}

void QueueFringe::push(searchNode* n) {
	f->addLast(n);
}

bool QueueFringe::empty() {
	return f->empty();
}

searchNode* QueueFringe::top() {
	return f->removeFirst();
}

void QueueFringe::pop() {

}

int QueueFringe::size() {
	return f->size();
}
} /* namespace progression */
