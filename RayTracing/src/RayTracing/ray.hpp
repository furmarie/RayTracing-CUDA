#ifndef RAYH
#define RAYH

#include "cuda_includes.h"

class ray {
public:
	__device__ ray() {}
	__device__ ray(const vec3& p1, const vec3& p2) {
		m_point1 = p1;
		m_point2 = p2;
		m_AB = p2 - p1;
	}

	__device__ vec3 getPoint1() {
		return m_point1;
	}
	__device__ vec3 getPoint2() {
		return m_point2;
	}

public:
	vec3 m_point1, m_point2;
	vec3 m_AB; // Vector goes from A to B
};


#endif