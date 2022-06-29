#ifndef OBJECTBASE_H
#define OBJECTBASE_H

#include "../cuda_includes.h"
#include "../ray.hpp"
#include "../math/gtform.hpp"


class objectBase;

struct hitRecord {
	vec3 intPoint;
	vec3 localNormal;
	vec3 localColour;
	objectBase* hitObj;
};


class objectBase {
public:
	// Constructor and destructor
	__device__ objectBase() {
		m_baseColour = vec3({0.0f, 1.0f, 1.0f});
		m_transformMatrix = nullptr;		
	}
	__device__ ~objectBase() {
		if(m_transformMatrix) {
			delete m_transformMatrix;
		}
	}

	__device__ virtual bool checkIntersection(
		const ray& r,
		hitRecord& record
	) const = 0;

	__device__ bool isClose(float a, float b) const {
		const float epsilon = 1e-9f;
		return fabs(a - b) < epsilon;
	}

	__device__ void setTransformMatrix(GTForm* transformMatrix) {
		m_transformMatrix = transformMatrix;
	}

public:
	vec3 m_baseColour;
	GTForm* m_transformMatrix;
};



#endif // !OBJECTBASE_H
