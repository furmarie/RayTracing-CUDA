#ifndef PLANE_H
#define PLANE_H

#include "objectbase.hpp"

class plane : public objectBase {

public:
	// Default constructor
	__device__ plane() {}

	// Override the destructor
	__device__ ~plane() {
		delete m_transformMatrix;
	}

	// Override the function to check for intersections
	__device__ virtual bool checkIntersection(
		const ray& r,
		hitRecord& record
	) const override {
		// Transform ray to local coordinates
		ray bckRay = m_transformMatrix->apply(r, BCK_TFM);

		// Get direction vector k
		vec3 k = normalize(bckRay.m_AB);

		// If ray parallel to plane no intersection
		if(isClose(k.z, 0.0)) {
			return false;
		}

		float t = -(bckRay.m_point1.z / k.z);
		if(t < 0.0) {
			return false;
		}

		// Compute u v values that are coordinates on plane
		vec3 poi = bckRay.m_point1 + t * k;
		//float u = bckRay.m_point1.x + t * k.x;
		//float v = bckRay.m_point1.y + t * k.y;

		if(fabs(poi.x) >= 1.0 || fabs(poi.y) >= 1.0) {
			return false;
		}
		//printf("%.2f\n", t);
		// Transform point of intersection to world coordinates
		record.intPoint = m_transformMatrix->apply(poi, FWRD_TFM, false);
		//printf("%.2f", record.intPoint.z);
		vec3 normal({ 0.0, 0.0, -1.0 });
		//vec3 localOrigin({ 0.0, 0.0, 0.0 });
		//vec3 globalOrigin = m_transformMatrix->apply(localOrigin, FWRD_TFM, false);
		record.localNormal = m_transformMatrix->apply(normal, FWRD_TFM, true); 
		record.localNormal = normalize(record.localNormal);
		
		// Invert localNormal if ray direction of castRay is the same
		if(dot(r.m_AB, record.localNormal) > 0.0) {
			record.localNormal *= -1;
		}

		// Return base colour
		record.localColour = m_baseColour;

		return true;
	}

private:

};

#endif