#ifndef MATERIALBASE_H
#define MATERIALBASE_H

#include "../cuda_includes.h"
#include "../primitives/objectbase.hpp"
#include "../lights/lightbase.hpp"
#include "listcontainer.hpp"

class materialBase {
public:
	__device__ materialBase() {
		m_hasTexture = false;
	}
	__device__ virtual ~materialBase() {
	}

	// Function to return colour at a point with this material
	virtual vec3 getColour(
		objectList** objList,
		lightList** lights

	) {

	}


public:
	bool m_hasTexture;
};


#endif // !MATERIALBASE_H
