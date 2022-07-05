#ifndef SIMPLEMATERIAL_H
#define SIMPLEMATERIAL_H

#include "materialbase.hpp"

class simpleMaterial : public materialBase {
	// Constructor
public:
	__device__ simpleMaterial() {
		m_baseColour.y = 1.0f;
		m_reflectivity = 0.0f;
		m_shininess = 0.0f;
	}

	// Function to get the colour without reflective components
	__device__ vec3 getColour(
		objectList** objList,
		lightList** lights,
		hitRecord& record,
		const ray& cameraRay,
		float& reflectivity
	) const override {
		vec3 difColour{}, spcColour{};

		if(!m_hasTexture) {
			difColour = computeDiffuseColour(
				objList,
				lights,
				record,
				m_baseColour
			);
		}
		else {
			// Do nothing for now
			// Add when there are textures
		}

		// Compute the specular component
		if(m_shininess > 0.0) {
			printf("Spec\n");
			spcColour = computeSpecular(
				objList,
				lights,
				record,
				cameraRay
			);
		}

		// Compute colour components to return
		vec3 res = spcColour + difColour * (1 - m_reflectivity);

		// Return reflectivity of this material
		reflectivity = m_reflectivity;

		return res;
	}

	// Function to compute specular highlights
	__device__ vec3 computeSpecular(
		objectList** objList,
		lightList** lights,
		hitRecord& record,
		const ray& cameraRay
	) const {
		vec3 spcColour{};
		if(m_shininess == 0.0f) {
			return spcColour;
		}

		// Loop through all the lights in the scene
		for(int i = 0; i < (*lights)->numItems; i++) {
			lightBase* currLight = (*lights)->list[i];
			// Check for intersections with all objects in the scene
			float intensity = 0.0;
			// Construct a vector from intersection point to the light
			vec3 lightDir = normalize(currLight->m_location - record.intPoint);

			// Compute a start point
			vec3 startPoint = record.intPoint + (lightDir * 0.001);

			// Construct a ray from the point of intersection to the light
			ray lightRay(startPoint, startPoint + lightDir);

			// Loop through all objects in the scene to check if any object
			// obstructs light from this source
			hitRecord tempRec;
			bool validInt = false;
			for(int j = 0; j < (*objList)->numItems; j++) {
				objectBase* sceneObj = (*objList)->list[i];
				if(sceneObj == record.hitObj) {
					continue;
				}
				validInt = sceneObj->checkIntersection(lightRay, tempRec);

				// If no intersections were found, proceed with calculating
				// specular component
				if(validInt) {
					break;
				}
			} // Done with object list

			// Compute reflection vector
			vec3 d = lightRay.m_AB;
			vec3 r = d - (2.0f * dot(d, record.localNormal) * record.localNormal);
			r = normalize(r);

			// Compute the dot product
			vec3 v = normalize(cameraRay.m_AB);
			float dotProduct = dot(r, v);

			// Only proceed if dot product is positive
			// i.e viewing angle and reflected ray are in same direction
			if(dotProduct > 0.0) {
				intensity = m_reflectivity * powf(dotProduct, m_shininess);
			}
			spcColour += currLight->m_baseColour * intensity;
			//red += currLight->m_baseColour[0] * intensity;
			//green += currLight->m_baseColour[1] * intensity;
			//blue += currLight->m_baseColour[2] * intensity;
		} // Done checking all lights
		//spcColour[0] = red;
		//spcColour[1] = blue;
		//spcColour[2] = green;
		//return vec3({ red, blue, green });
		return spcColour;
	}

public:
	float m_reflectivity;
	float m_shininess=10.0;

};

#endif // !SIMPLEMATERIAL_H
