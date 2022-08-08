#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "lightbase.hpp"

class pointLight : public lightBase {
public:
	// Constructor and Destructor
	__device__ pointLight() {
		m_baseColour = vec3({ 1.0, 1.0, 1.0 });
		m_location = vec3({0.0, 0.0, -6.0});
		m_intensity = 1.0f;
	}
	__device__ ~pointLight() {}

	// Function to compute illumination
	__device__ virtual bool computeIllumination(
		const hitRecord& record,
		objectBase* currObj,
		objectList** objList,
		vec3& colour,
		float& intensity
	) const override {
		// Make a vector from int point to this light
		vec3 lightDir = normalize(m_location - record.intPoint);

		// Compute a starting point
		vec3 startPoint = record.intPoint;

		ray lightRay(startPoint, startPoint + lightDir);

		// Check for intersection with other objects in the scene
		hitRecord temp;
		bool validInt = false;
		float lightDist = length(m_location - startPoint);

		for(int i = 0; i < (*objList)->numItems; i++) {
			if((*objList)->list[i] != currObj) {
				validInt = (*objList)->list[i]->checkIntersection(lightRay, temp);
				if(validInt) {
					float dist = length(temp.intPoint - startPoint);
					if(dist > lightDist) {
						validInt = false;
					}
					else {
						// There is an intersection which means shadow is being
						// cast by this light on currObj
						break;
					}
				}
			}
		}

		if(validInt) {
			// No illumination from this source
			colour = m_baseColour;
			intensity = 0.0f;
			return false;
		}

		// Compute illumination 
		float theta = acosf(dot(record.localNormal, lightDir));
		if(theta > PI_2) {
			colour = m_baseColour;
			intensity = 0.0f;
			return false;
		}
		else {
			// Illumination is there
			colour = m_baseColour;
			intensity = m_intensity * (1.0f - theta / PI_2);
			return true;
		}
		return false;
	}

public:

};

#endif
