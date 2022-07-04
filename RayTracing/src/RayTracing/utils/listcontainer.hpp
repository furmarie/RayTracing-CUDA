#ifndef LISTCONTAINER_H
#define LISTCONTAINER_H

// Contains list of pointers to object of type T
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

	__device__ bool addItem(T* item) {
		if(numItems >= maxItems) {
			return false;
		}
		list[numItems++] = item;
		return true;
	}

	__device__ T* operator[] (size_t idx) {
		if(idx >= numItems) {
			return nullptr;
		}
		return list[idx];
	}

public:
	T** list;
	int numItems;
	int maxItems;
};


#endif // !LISTCONTAINER_H
