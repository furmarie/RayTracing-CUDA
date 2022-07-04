#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

#include "Walnut/Image.h"

#include "camera.hpp"
#include "primitives/sphere.hpp"
#include "primitives/plane.hpp"


//#include "primitives/cylinder.hpp"
//#include "primitives/cone.hpp"
//#include "primitives/torus.hpp"
#include "lights/pointlight.hpp"

#include "listcontainer.hpp"


#include "cuda_includes.h"

namespace fRT {
	class Scene {
	public:
		// Default constructor
		Scene();

		~Scene();

		// Function to actually render
		bool Render();

		bool onInit();

		// Function to cast a ray into the scene
		//bool castRay(
		//	ray& r,
		//	hitRecord& record
		//);

		// KeyBoard key press handlers

		// Move forwards, backwards and sideways
		bool keyPressedW();
		bool keyPressedA();
		bool keyPressedS();
		bool keyPressedD();

		// Camera rotation, pitch and yaw
		bool keyPressedUpArrow();
		bool keyPressedDownArrow();
		bool keyPressedLeftArrow();
		bool keyPressedRightArrow();

		// Reset camera position
		bool keyPressedR();

		// Move up and down from current position
		bool keyPressedSpace();
		bool keyPressedLCtrl();

		// Handle mouse movement
		bool mouseMoved(vec2 currPos, bool moveCamera);

		// Handle colour changes
		bool handleColours(vec3& sphereCol);

		void OnResize(uint32_t width, uint32_t height);

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

		// Private functions
	private:

		// Private members
	private:
		// Camera for this scene
		camera m_camera;

		// Previous mouse position
		vec2 prevMousePos;

		// Image Buffers for device and host
		vec3* m_deviceImageBuffer;
		vec3* m_hostImageBuffer;

		bool m_changedState = true;
		bool m_worldChanged = true;

		// Testing camera position change with key press
		vec3 m_cameraPosition;
		float m_cameraStep = 0.05f;
		float m_cameraAngleStep = 0.02f;

		// Testing colour change for just one sphere
		vec3 sphereColour;

		// List for objects and lights. Pointers on device
		objectList** d_objList;
		lightList** d_lightList;
		objectBase** d_world;
		lightBase** d_lights;

		// Vectors for objects and lights and device

		// Camera object for this scene
		camera** d_camera;

		// Variables to hold amount of camera movement per frame
		float dUp = 0.0f; // In literal up direction, camera Up
		float dRight = 0.0f; // In right direction, 90 degrees to lookUp
		
		float dForward = 0.0f; // In direction of alignment vector

		float dTheta = 0.0f;
		float dPhi = 0.0f;

		std::shared_ptr<Walnut::Image> m_FinalImage;
		uint32_t* m_ImageData = nullptr;

		bool doneInitialise = false;
		bool m_resetCamera = false;

		// List of all objects in the scene
		//std::vector<std::shared_ptr<objectBase>> m_world;

		// List of all lights in the scene
		//std::vector<std::shared_ptr<lightBase>> m_lights;
	};
};


#endif