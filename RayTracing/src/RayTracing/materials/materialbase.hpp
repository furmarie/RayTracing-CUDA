#ifndef MATERIALBASE_H
#define MATERIALBASE_H

#include "../cuda_includes.h"
#include "../primitives/objectbase.hpp"
#include "../lights/lightbase.hpp"

class materialBase {
public:
	__device__ materialBase() {
		m_hasTexture = false;
	}
	__device__ virtual ~materialBase() {
	}

	// Function to return colour at a point with this material
	virtual vec3 getColour(
		objectBase** objectList,

		lightBase** lightList,
	) {

	}


public:
	bool m_hasTexture;
};

#endif // !MATERIALBASE_H
