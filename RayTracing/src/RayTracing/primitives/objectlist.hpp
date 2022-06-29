#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include "objectbase.hpp"

class objectList {
public:
	__device__ objectList(objectBase** _l, int _nItems) {
		list = _l;
		numItems = 0;
		maxItems = _nItems;
	}

	//__device__ ~objectList() {
	//	for(int i = 0; i < numItems; i++) {
	//		delete list[i];
	//	}
	//}

	__device__ bool addItem(objectBase** item) {
		if(numItems == maxItems) {
			return false;
		}
		list[numItems++] = *item;
		return true;
	}

public:
	objectBase** list;
	int numItems;
	int maxItems;
};


#endif // !OBJECTLIST_H
