#ifndef GTFORM_H
#define GTFORM_H


#include "matrix4x4.h"
#include "../ray.hpp"



constexpr bool FWRD_TFM = true;
constexpr bool BCK_TFM = false;

class GTForm {
	// Constructor and Destructor
public:
	__device__ GTForm() {
		setTransform(
			vec3({0, 0, 0}),
			vec3({0, 0, 0}),
			vec3({1, 1, 1})
		);
	}
	__device__ ~GTForm() {}

	// Construct from matrices
	__device__ GTForm(const matrix4f& fwd, const matrix4f& bck) {
		m_fwd = fwd;
		m_bck = bck;
	}

	__device__ GTForm(
		const vec3& translation,
		const vec3& rotation,
		const vec3& scale
	) {
		setTransform(
			translation,
			rotation,
			scale
		);
	}

	__device__ void setTransform(
		const vec3& translation,
		const vec3& rotation,
		const vec3& scale
	) {
		// No component in scale can be zero

		// Matrices for separate components
		matrix4f translationMatrix, scaleMatrix;
		matrix4f rotationMatrixX, rotationMatrixY, rotationMatrixZ;


		// Fill translation and scale matrices
		translationMatrix.element(0, 3) = translation.x;
		translationMatrix.element(1, 3) = translation.y;
		translationMatrix.element(2, 3) = translation.z;

		scaleMatrix.element(0, 0) = scale.x;
		scaleMatrix.element(1, 1) = scale.y;
		scaleMatrix.element(2, 2) = scale.z;


		// Fill rotation matrix
		float theta_x = rotation.x;
		float theta_y = rotation.y;
		float theta_z = rotation.z;

		rotationMatrixX.element(1, 1) = cosf(theta_x);
		rotationMatrixX.element(1, 2) = -sinf(theta_x);
		rotationMatrixX.element(2, 1) = sinf(theta_x);
		rotationMatrixX.element(2, 2) = cosf(theta_x);

		rotationMatrixY.element(0, 0) = cosf(theta_y);
		rotationMatrixY.element(0, 2) = sinf(theta_y);
		rotationMatrixY.element(2, 0) = -sinf(theta_y);
		rotationMatrixY.element(2, 2) = cosf(theta_y);

		rotationMatrixZ.element(0, 0) = cosf(theta_z);
		rotationMatrixZ.element(0, 1) = -sinf(theta_z);
		rotationMatrixZ.element(1, 0) = sinf(theta_z);
		rotationMatrixZ.element(1, 1) = cosf(theta_z);

		// Combine to get fwd transform matrix
		m_fwd = (translationMatrix *
			((rotationMatrixX *
				(rotationMatrixY *
					rotationMatrixZ)) *
				scaleMatrix));

		// Compute backward transform
		m_bck = inverse(m_fwd);
	}

	// Get fwd and bck transform
	__device__ matrix4f getForward() {
		return m_fwd;
	}
	__device__ matrix4f getBackward() {
		return m_bck;
	}

	// Function to apply transform
	__device__ ray apply(const ray& inputRay, bool dirFlag) const {
		// Create result
		ray res;

		res.m_point1 = apply(inputRay.m_point1, dirFlag, false);
		res.m_point2 = apply(inputRay.m_point2, dirFlag, false);
		res.m_AB = res.m_point2 - res.m_point1;

		return res;
	}

	__device__ vec3 apply(const vec3& inputVec, bool dirFlag, bool isDirection) const {
		// Convert inputVec to 4 element vector
		// If isDirection no need of applying translate
		// If point to apply translate set w = 1
		vec4 temp({ inputVec.x, inputVec.y, inputVec.z, (float) !isDirection });

		// Apply transform as per flag
		vec4 tempRes = (dirFlag ? m_fwd : m_bck) * temp;
		vec3 res({ tempRes.x, tempRes.y, tempRes.z });
		return res;
	}

public:
	matrix4f m_fwd, m_bck;
};


#endif // !GTFORM_H
