#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include "objectbase.hpp"

template<class T>
class listContainer {
public:
	__device__ listContainer(T** _l, int _nItems) {
		list = _l;
		numItems = 0;
		maxItems = _nItems;
	}

	__device__ ~listContainer() {
		for(int i = 0; i < numItems; i++) {
			delete list[i];
		}
	}

	__device__ bool addItem(T** item) {
		if(numItems >= maxItems) {
			return false;
		}
		list[numItems++] = *item;
		return true;
	}

public:
	T** list;
	int numItems;
	int maxItems;
};


#endif // !OBJECTLIST_H
