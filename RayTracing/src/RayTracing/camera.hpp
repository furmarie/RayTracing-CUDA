#ifndef CAMERA_H
#define CAMERA_H

#include "cuda_includes.h"
#include "ray.hpp"


class camera {
public:
	// Default constructor
	__device__ camera() {
	// Assuming screen is along +ve Y axis, and UP is along +ve Z axis
		m_cameraPosition = vec3({ 0.0, -10.0, 0.0 });
		m_cameraLookAt = vec3({ 0.0, 0.0, 0.0 });
		m_cameraUp = vec3({ 0.0, 0.0, 1.0 });
		m_cameraFocalLength = 1.0;
		m_cameraHorzSize = 1.0;
		m_cameraAspectRatio = 1.0;
	}

	__device__ camera(
		const vec3& newPos,
		const vec3& newLookAt,
		const vec3& newUp,
		float newFocalLength,
		float newHorzSize,
		float newAspectRatio
	) {
		setPosition(newPos);
		setLookAt(newLookAt);
		setUp(newUp);
		setFocalLength(newFocalLength);
		setHorzSize(newHorzSize);
		setAspectRatio(newAspectRatio);
	}

	// Functions for setting camera parameters
	__device__ void setPosition(const vec3& newPos) {
		m_cameraPosition = newPos;
	}
	__device__ void setLookAt(const vec3& newLookAt) {
		m_cameraLookAt = newLookAt;
	}
	__device__ void setUp(const vec3& newUp) {
		m_cameraUp = normalize(newUp);
	}

	__device__ void setFocalLength(float newFocalLength) {
		m_cameraFocalLength = newFocalLength;
	}

	__device__ void setHorzSize(float newHorzSize) {
		m_cameraHorzSize = newHorzSize;
	}

	__device__ void setAspectRatio(float newAspectRatio) {
		m_cameraAspectRatio = newAspectRatio;
	}

	// Method to return camera position
	__device__ vec3 getPosition() const {
		return m_cameraPosition;
	}

	// Method to return LookAt
	__device__ vec3 getLookAt() const {
		return m_cameraLookAt;
	}

	// Method to return CameraUp
	__device__ vec3 getUp() const {
		return m_cameraUp;
	}

	// Method to return focal length
	__device__ float getFocalLength() const {
		return m_cameraFocalLength;
	}

	// Method to return camera horizontal screen size
	__device__ float getHorzSize() const {
		return m_cameraHorzSize;
	}

	// Method to return camera vertical screen size
	__device__ float getVertSize() const {
		return m_cameraVertSize;
	}

	// Method to return aspect ratio
	__device__ float getAspectRatio() const {
		return m_cameraAspectRatio;
	}

	// Method to return U vector
	__device__ vec3 getU() const {
		return m_projectionScreenU;
	}

	// Method to return V vector
	__device__ vec3 getV() const {
		return m_projectionScreenV;
	}

	// Method to return screen centre
	__device__ vec3 getScreenCentre() const {
		return m_projectionScreenCentre;
	}

	// Function to generate a ray
	__device__ bool generateRay(float proScreenX, float proScreenY, ray& cameraRay) {
	// Compute location of screen point from UV system to world system

		vec3 screenWorldPoint = m_projectionScreenCentre +
			(m_projectionScreenU * proScreenX) + (m_projectionScreenV * proScreenY);

		// Use this point with camera position to calculate ray
		cameraRay.m_point1 = m_cameraPosition;
		cameraRay.m_point2 = screenWorldPoint;
		cameraRay.m_AB = screenWorldPoint - m_cameraPosition;
		return true;
	}

	// Function to set camera transform
	//__device__ void setTransform(GTform* transformMatrix) {

	//}

	// Function to rotate the alignment vector
	// This only allows rotation horizontally and vertically
	// For now we also change the lookAt, but not necessary
	// theta [-PI / 2, PI / 2]
	// phi [-PI / 2, PI / 2]
	// Have to change later, add rotation to lookUp direction too
	__device__ void rotateAlignment(float theta, float phi) {
		if(theta == 0.0f && phi == 0.0f) {
			return;
		}
		// Theta needs to be between [-PI, 0]
		// Phi needs to be between [-PI / 2, PI / 2]

		float lookAtDist = length(m_cameraLookAt - m_cameraPosition);
		vec3 U = normalize(cross(m_alignmentVector, m_cameraUp));
		vec3 V = normalize(cross(m_projectionScreenU, m_alignmentVector));

		// Alignment vector component due to both theta and phi
		float alignmentVecComp = cosf(theta) + cosf(phi);

		// V vector comp due to theta only
		float vVecComp = sinf(phi);

		// U vector comp due to phi only
		float uVecComp = sinf(theta);

		vec3 newAlignmentDir = normalize(alignmentVecComp * m_alignmentVector + vVecComp * V + uVecComp * U);
		m_cameraLookAt = newAlignmentDir * lookAtDist; // New lookAt

		UpdateCameraGeometry();
	}

	// To move camera move its position in the world
	// u is for moving Right and Left. positive is to the right
	// Moves to the right 90 degree from cameraUp
	__device__ void moveRight(float u) {
		vec3 U = normalize(cross(m_alignmentVector, m_cameraUp));
		//vec3 V = normalize(cross(m_projectionScreenU, m_alignmentVector));

		vec3 diffVect = u * U;
		m_cameraPosition += diffVect;
		m_cameraLookAt += diffVect;
		UpdateCameraGeometry();
	}

	// Literally move up, i.e cameraUp
	__device__ void moveVertically(float dUp) {
		vec3 upDir = m_cameraUp;

		vec3 diffVect = dUp * upDir;
		m_cameraPosition += diffVect;
		m_cameraLookAt += diffVect;
		UpdateCameraGeometry();
	}

	// Move forward in the direction of alignment vector
	// i.e the direction camera is pointing at
	__device__ void moveForward(float dForward) {
		vec3 forwardDir = m_alignmentVector;

		vec3 diffVect = dForward * forwardDir;
		m_cameraPosition += diffVect;
		UpdateCameraGeometry();
	}

	// Function to update the camera geometry
	__device__ void UpdateCameraGeometry() {
		m_cameraVertSize = (m_cameraHorzSize / m_cameraAspectRatio);

		// First, compute the vector from the camera position to the LookAt position.
		m_alignmentVector = m_cameraLookAt - m_cameraPosition;
		m_alignmentVector = normalize(m_alignmentVector);


		// Second, compute the U and V vectors.
		m_projectionScreenU = normalize(cross(m_alignmentVector, m_cameraUp));
		m_projectionScreenV = normalize(cross(m_projectionScreenU, m_alignmentVector));

		// Thirdly, compute the positon of the centre point of the screen.
		m_projectionScreenCentre = m_cameraPosition + (m_cameraFocalLength * m_alignmentVector);

		// Modify the U and V vectors to match the size and aspect ratio.
		m_projectionScreenU = m_projectionScreenU * m_cameraHorzSize;
		m_projectionScreenV = m_projectionScreenV * m_cameraVertSize;

	}

private:
	vec3 m_cameraPosition; // Origin of camera
	vec3 m_cameraLookAt; // Point camera looks at
	vec3 m_cameraUp; // Direction defining UP for the camera
	float m_cameraFocalLength;
	float m_cameraHorzSize;  // Width
	float m_cameraVertSize;  // Height
	float m_cameraAspectRatio; // Width / Height

	vec3 m_alignmentVector;
	vec3 m_projectionScreenU;
	vec3 m_projectionScreenV;
	vec3 m_projectionScreenCentre;
};



#endif