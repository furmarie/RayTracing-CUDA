#ifndef SPHERE_H
#define SPHERE_H

#include "objectbase.hpp"

class sphere : public objectBase {
public:
	// Constructor and Destructor
	__device__ sphere() {
		m_baseColour = vec3({1.0f, 0.7f, 0.05f});
	}
	__device__ ~sphere() {
		delete m_transformMatrix;
	}

	// Function to check intersection
	__device__ virtual bool checkIntersection(
		const ray& r,
		hitRecord& record
	) const override {
		// Copy ray and apply backward transform on ray to convert it to
		// local coordinate system

		ray bckRay = m_transformMatrix->apply(r, BCK_TFM);


		vec3 dir = normalize(bckRay.m_AB);

		float b = 2.0f * dot(bckRay.m_point1, dir);
		float c = dot(bckRay.m_point1, bckRay.m_point1) - 1.0f;

		float disc = b * b - 4.0f * c;

		if(disc > 0.0) {
			float sqrtDisc = sqrtf(disc);
			float t1 = 0.5f * (-b - sqrtDisc);
			float t2 = 0.5f * (-b + sqrtDisc);
			//printf("%.2f\n", t1);
			if(t1 < 0.0 || t2 < 0.0) {
				return false;
			}
			else {
				vec3 poi;
				t1 = fmin(t1, t2);

				poi = bckRay.m_point1 + t1 * dir;

				// Transform the poi to world coordinate system
				record.intPoint = m_transformMatrix->apply(poi, FWRD_TFM, false);

				vec3 normal = poi; //poi - center which is 0, 0, 0
				//vec3 localOrigin({0.0, 0.0, 0.0});
				//vec3 globalOrigin = m_transformMatrix->apply(localOrigin, FWRD_TFM, false);
				record.localNormal = m_transformMatrix->apply(normal, FWRD_TFM, true); // set isDirection to true
				record.localNormal = normalize(record.localNormal);
				
				// Return colour
				record.localColour = m_baseColour;
				return true;
			}
		}
		return false;
	}

public:

};

#endif // !SPHERE_H
