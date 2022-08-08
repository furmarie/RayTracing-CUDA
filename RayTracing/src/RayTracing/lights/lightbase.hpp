#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "../cuda_includes.h"
#include "../primitives/objectbase.hpp"
#include "listcontainer.hpp"

class lightBase {
public:
	// Constructor and Destructor
	__device__ lightBase() {
		m_baseColour = vec3({1.0, 1.0, 1.0});
		m_location = vec3({0.0, 0.0, -1.0});
		m_intensity = 1.0;
	}
	__device__ ~lightBase() {}

	// Function to compute illumination
	__device__ virtual bool computeIllumination(
		const hitRecord& record,
		objectBase* currObj,
		objectList** objList,
		vec3& colour,
		float& intensity
	) const = 0;

public:
	vec3 m_baseColour;
	vec3 m_location;
	float m_intensity;
};

using lightList = listContainer<lightBase>;

#endif
