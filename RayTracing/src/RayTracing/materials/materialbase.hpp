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
	__device__ virtual vec3 getColour(
		objectList** objList,
		lightList** lights,
		hitRecord& record,
		const ray& cameraRay,
		float& reflectivity
	) const {
		reflectivity = 0.0f;
		return { 1.0, 1.0, 0.0 };
	}

	// Function to compute diffuse colour
	__device__ static vec3 computeDiffuseColour(
		objectList** objList,
		lightList** lights,
		hitRecord& record,
		const vec3& baseColour
	) {
		// Compute colour due to diffuse illumination
		vec3 diffuseColour{}, tempColour{};
		vec3 colour;
		float intensity = 0.0f;
		bool validIllum = false;
		bool illumFound = false;

		for(int i = 0; i < (*lights)->numItems; i++) {
			validIllum = (*lights)->list[i]->computeIllumination(
				record,
				record.hitObj,
				objList,
				colour,
				intensity
			);
			if(validIllum) {
				illumFound = true;
				tempColour += colour * intensity;
			}
		}

		if(illumFound) {
			diffuseColour = tempColour * baseColour;
		}
		// Return colour of material
		return diffuseColour;
	}

public:
	vec3 m_baseColour;
	bool m_hasTexture;
};


#endif // !MATERIALBASE_H
