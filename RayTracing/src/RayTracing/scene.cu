#include <chrono>
//#include <thread>
//
//#include "ctpl.h"
#include "scene.cuh"
#include "ray.hpp"
#include "materials/simplematerial.hpp"
//#include "textures/checker.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>


#define checkCudaErrors1(val) check_cuda1( (val), #val, __FILE__, __LINE__ )
void check_cuda1(cudaError_t result, char const* const func, const char* const file, int const line) {
	if(result) {
		std::cerr << "CUDA error = " << static_cast<uint32_t>(result) << " at " <<
			file << ":" << line << " '" << func << "' \n";
		auto errName = cudaGetErrorName(result);
		std::cerr << errName << '\n';
		// Make sure we call CUDA Device Reset before exiting
		cudaDeviceReset();
		exit(99);
	}
}


//__device__
//vec3 pixel_func(const int x, const int y, const int xSize, const int ySize, camera** cam) {
//	float xFact = 1.0 / (static_cast<float>(xSize) / 2.0);
//	float yFact = 1.0 / (static_cast<float>(ySize) / 2.0);
//	float minDist = 1e6;
//	float maxDist = 0.0;
//
//	float normX = (static_cast<float>(x) * xFact) - 1.0;
//	float normY = (static_cast<float>(y) * yFact) - 1.0;
//
//	// Check each pixel in image and send ray out to that
//	// and check if there is an intersection
//	fRT::ray cameraRay;
//
//	// Generate the ray for this pixel.
//	(*cam)->generateRay(normX, normY, cameraRay);
//
//	// Hold records of closest hit object
//	//hitRecord record;
//	//std::shared_ptr<objectBase> currObj = nullptr;
//
//	int currDepth = 0;
//
//	vec3 colour;
//
//	// Attenuates next colour after reflection
//	float attenuation = 1.0;
//
//	vec3 cameraScreenCentre = (*cam)->getScreenCentre();
//	/*
//	while(currDepth++ < maxReflectionDepth and attenuation > 0.0) {
//		bool intersectionFound = materialBase::castRay(
//			cameraRay,
//			m_world,
//			currObj,
//			record
//		);
//
//
//
//		// Compute illumination for closest object if intersectionFound
//		if(intersectionFound) {
//			//exit(0);
//			currObj = record.obj;
//			// Check if the closest object has a material
//			if(currObj->m_hasMaterial) {
//				// Get colour components without reflective component of this material
//				float currObjReflectivity = 0.0;
//				vec3 colourComps = record.obj->m_pMaterial->getColour(
//					m_world,
//					m_lights,
//					record,
//					cameraRay,
//					currObjReflectivity
//				);
//
//				colour += attenuation * colourComps;
//				attenuation *= currObjReflectivity;
//
//				// Change cameraRay to reflected ray at this position
//				// Compute the reflection vector
//				vec3 d = cameraRay.m_AB;
//				vec3 reflectedVector = d - 2 * (vec3::dot(d, record.localNormal)) * record.localNormal;
//
//				// Construct the reflection ray
//				vec3 startPoint = record.intPoint;
//				ray reflectedRay(startPoint, startPoint + reflectedVector);
//				cameraRay = reflectedRay;
//			}
//			else {
//				// Use basic method to compute colour as object has no material
//				vec3 matColour = materialBase::computeDiffuseColour(m_world, m_lights, record, record.obj->m_baseColour);
//
//				colour += attenuation * matColour;
//
//				break; // NO more reflection
//
//				//outputImage.SetPixel(x, y, matColour[0], matColour[1], matColour[2]);
//			}
//		}
//		else {
//			// No intersection after this
//			//
//			// Get Sky Colour from direction of ray
//
//			// Direction vector is somewhere on projection screen
//			// The Z value ranges from centre - VertSize to centre + VertSize
//			float vertSize = m_camera.getVertSize();
//			auto z = cameraRay.m_AB[2];
//			float t = (z - (cameraScreenCentre[2] - vertSize)) / (2 * vertSize);
//
//			//float t = 0.5 * (-dir_unit_vec[2] + 1.0);
//			//if(t < 0.0 or t > 1.0) {
//			//	std::cerr << t << '\n';
//			//	exit(0);
//			//}
//
//			t = fmax(t, 0.0);
//			t = fmin(t, 1.0);
//
//			//std::cerr << t << std::endl;
//			//if(x == xSize - 2 and y < ySize - 10) {
//			//	exit(0);
//			//}
//
//			mn = fmin(mn, t);
//			mx = fmax(mx, t);
//
//			vec3 initial({ 1, 1, 1 });
//			vec3 final_colour({ 0.5, 0.7, 1.0 });
//			colour += attenuation * (t * initial + (1 - t) * final_colour);
//			break;
//		}
//	}
//	*/
//
//	// Get Sky Colour from direction of ray
//
//			// Direction vector is somewhere on projection screen
//			// The Z value ranges from centre - VertSize to centre + VertSize
//	float vertSize = (*cam)->getVertSize();
//	auto z = cameraRay.m_AB.z;
//	float t = (z - (cameraScreenCentre.z - vertSize)) / (2 * vertSize);
//
//	//float t = 0.5 * (-dir_unit_vec[2] + 1.0);
//	//if(t < 0.0 or t > 1.0) {
//	//	std::cerr << t << '\n';
//	//	exit(0);
//	//}
//	if(t < 0.0) {
//		t = 0.0;
//	}
//	if(t > 1.0) {
//		t = 1.0;
//	}
//
//
//	vec3 initial({ 1, 1, 1 });
//	vec3 final_colour({ 0.5, 0.7, 1.0 });
//	colour += attenuation * (t * initial + (1 - t) * final_colour);
//
//	return colour;
//
//}

__device__ bool castRay(const ray& r, hitRecord& record, objectList** objList) {
	float minDist = 1e6;
	float maxDist = 0.0;

	bool intersectionFound = false;
	hitRecord temp;
	for(int i = 0; i < (*objList)->numItems; i++) {
		//printf("HERE 4\n");

			//if(i == 1) {
			//	printf("PLANE\n");
			//	
			//}
		if((*objList)->list[i]->checkIntersection(r, temp)) {
			//if(i == 0) {
			//	//printf("HIT PLANE");
			//	record.localNormal = vec3({0, +1, 0});
			//	record.hitObj = world[i];
			//	record.localColour = vec3({1,1,1});
			//	return true;
			//}
			intersectionFound = true;
			float dist = length(temp.intPoint - r.m_point1);
			if(dist < minDist) {
				minDist = dist;
				record = temp;
				record.hitObj = (*objList)->list[i];
			}
		}
	}
	return intersectionFound;
}

__device__ vec3 pixel_colour(const int x, const int y,
	const int xSize, const int ySize,
	camera** cam,
	objectList** d_objList,
	lightList** d_lightList
) {
	//return vec3({0, 1, 0});

	float xFact = 1.0 / (static_cast<float>(xSize) / 2.0);
	float yFact = 1.0 / (static_cast<float>(ySize) / 2.0);


	float normX = (static_cast<float>(x) * xFact) - 1.0;
	float normY = (static_cast<float>(y) * yFact) - 1.0;

	ray cameraRay;

	(*cam)->generateRay(normX, normY, cameraRay);

	hitRecord record;

	// DEBUGGING


	// END DEBUGGING

	bool validInt = castRay(cameraRay, record, d_objList);

	if(validInt) {
		//return record.localColour;
		//printf("HIT PLANE \n");
		vec3 colour, difColour;
		float intensity = 0.0f;
		//bool validIllum = ((*d_lightList)->list[0])->computeIllumination(record, record.hitObj, objList, colour, intensity);
		//if(validIllum) {
		//	//colour *= intensity;
		//	//printf("%.2f\n", intensity);
		//	difColour = colour * intensity * record.localColour;
		//}
		//else {
		//	// Debugging shadows
		//	//return vec3({0, 1, 0});
		//}


		if(record.hitObj->m_hasMaterial) {
			float reflectivity = 0.0f;
			vec3 res = record.hitObj->m_pMaterial->getColour(
				d_objList,
				d_lightList,
				record,
				cameraRay,
				reflectivity
			);
			return res;
		}
		else {
			// Use basic method to compute colour because no material
			difColour = materialBase::computeDiffuseColour(
				d_objList,
				d_lightList,
				record,
				record.hitObj->m_baseColour
			);
		}

		return difColour;
	}
	else {
		//float vertSize = (*cam)->getVertSize();
		//auto z = cameraRay.m_AB.z;
		//float t = (z - ((*cam)->getScreenCentre().z - vertSize)) / (2 * vertSize);

		////float t = 0.5 * (-dir_unit_vec[2] + 1.0);
		////if(t < 0.0 or t > 1.0) {
		////	std::cerr << t << '\n';
		////	exit(0);
		////}

		//t = clamp(t, 0.0, 1.0);
		//vec3 initial({ 1.0, 1.0, 1.0 });
		//vec3 finl({ 0.5, 0.5, 1.0 });

		//vec3 res = t * initial + (1 - t) * finl;
		return vec3();
	}
}

__global__
void render(vec3* buff, const int xSize, const int ySize, camera** cam, objectList** d_objList, lightList** d_lightList) {
	//if(threadIdx.x == 0 && blockIdx.x == 0) {
	//	printf("HERE render<> %f\n", (*objList)->list[1]->m_baseColour.x);

	//}

	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	if(x >= xSize || y >= ySize) {
		return;
	}
	buff[y * xSize + x] = pixel_colour(x, y, xSize, ySize, cam, d_objList, d_lightList);
}

__global__
void create_world(objectList** objList, lightList** d_lightList, lightBase** d_lights, objectBase** d_world, vec3 sphereCol) {
	if(threadIdx.x == 0 && blockIdx.x == 0) {
		objectBase* sphere1 = new sphere();
		GTForm* sphereTform = new GTForm();
		sphereTform->setTransform(
			vec3({ 0.5, 0.5, 0.0 }),
			vec3({ 0.0, 0.0, 0.0 }),
			vec3({ 0.4, 0.4, 0.4 })
		);

		GTForm* planeTform = new GTForm();
		objectBase* plane1 = new plane();
		plane1->m_baseColour = vec3({ 0.5, 0.5, 0.5 });
		plane1->setTransformMatrix(
			new GTForm(
				vec3({ 0.0, 0.0, -1.0 }),
				vec3({ 0.0, 0.0, 0.0 }),
				vec3({ 3.0, 3.0, 3.0 })
			)
		);

		GTForm* sphereTform2 = new GTForm();
		sphereTform2->setTransform(
			vec3({ -2.5, 0.0, +0.2 }),
			vec3({ 0.0, 0.0, 0.0 }),
			vec3({ 0.5, 0.5, 0.5 })
		);
		sphere1->setTransformMatrix(sphereTform2);

		objectBase* sphere2 = new sphere();
		sphere2->m_baseColour = vec3({ 0.0, 0.7, 0.9 });
		sphere2->setTransformMatrix(sphereTform);

		(*objList) = new objectList(d_world, 5);

		objectBase* sphere3 = new sphere();
		sphere3->setTransformMatrix(planeTform);
		(*objList)->addItem(sphere2);
		(*objList)->addItem(sphere1);
		(*objList)->addItem(plane1);

		// Create a sample material
		simpleMaterial* sampleMat = new simpleMaterial();
		sampleMat->m_baseColour = { 1.0, 0.0, 0.0 };
		sampleMat->m_reflectivity = 0.0f;
		sampleMat->m_shininess = 10.0f;
		sphere2->setMaterial((materialBase*) sampleMat);

		(*d_lightList) = new lightList(d_lights, 2);

		lightBase* light1 = new pointLight();
		light1->m_location = vec3({ -5.0, -5.0, +5.0 });
		lightBase* light2 = new pointLight();
		light2->m_location = vec3({ +5.0, -5.0, +5.0 });
		//light2->m_baseColour = { 0.2, 0.7, 0.8 };
		(*d_lightList)->addItem(light1);
		(*d_lightList)->addItem(light2);
	}
}

__global__
void update_world(objectList** objList, lightList** d_lightList, objectBase** d_world, vec3 sphereCol) {
	// TODO create better interface to change object properties
	if(sphereCol.x > -0.5f) {
		if((*objList)->list[0]) {
			(*objList)->list[0]->m_baseColour = sphereCol;
		}
	}
}

__global__
void create_camera(camera** d_camera, vec3 cameraPosition, const int xSize, const int ySize) {
	if(threadIdx.x == 0 && blockIdx.x == 0) {
		//printf("CREATED CAMERA, xSize : %d , ySize : %d\n", xSize, ySize);
		if(!(*d_camera)) {
			(*d_camera) = new camera();
		}

		(*d_camera)->setPosition(cameraPosition);
		(*d_camera)->setLookAt(vec3{ 0.0, 0.0, 0.0 });
		(*d_camera)->setUp(vec3{ 0.0, 0.0, 1.0 });
		(*d_camera)->setHorzSize(0.25);
	}
}

__global__
void update_camera(camera** d_camera, const float dRight, const float dUp, const float dForward, const float theta, const float phi, const int xSize, const int ySize) {
	if(!(*d_camera)) {
		return;
	}
	(*d_camera)->setAspectRatio(((float) xSize) / ((float) ySize));
	(*d_camera)->UpdateCameraGeometry();
	(*d_camera)->moveRight(dRight);
	(*d_camera)->moveVertically(dUp);
	(*d_camera)->moveForward(dForward);
	(*d_camera)->rotateAlignment(theta, phi);
}



__global__
void free_world(camera** d_camera) {
	if(*d_camera) {
		delete* d_camera;
	}
}

__global__ 
void find_maximum_kernel(float* array, float* max, int* mutex, unsigned int n) {
	unsigned int index = threadIdx.x + blockIdx.x * blockDim.x;
	unsigned int stride = gridDim.x * blockDim.x;
	unsigned int offset = 0;

	extern __shared__ float cache[256];


	float temp = -1.0;
	while(index + offset < n) {
		temp = fmaxf(temp, array[index + offset]);

		offset += stride;
	}

	cache[threadIdx.x] = temp;

	__syncthreads();


	// reduction
	unsigned int i = blockDim.x / 2;
	while(i != 0) {
		if(threadIdx.x < i) {
			cache[threadIdx.x] = fmaxf(cache[threadIdx.x], cache[threadIdx.x + i]);
		}

		__syncthreads();
		i /= 2;
	}

	if(threadIdx.x == 0) {
		while(atomicCAS(mutex, 0, 1) != 0);  //lock
		*max = fmaxf(*max, cache[0]);
		atomicExch(mutex, 0);  //unlock
	}
}

// Temporary
// kernel to get pointer to first element in image buffer
__global__
void get_ptr(vec3* buff, float* resPtr, size_t n) {
	if(threadIdx.x == 0 && blockIdx.x == 0) {
		resPtr = &(buff[0].x);
		//printf("%.2f\n", *(resPtr + n - 1));
	}
}

namespace fRT {
	Scene::Scene() {
		m_changedState = true;
		m_worldChanged = true;
		m_cameraPosition = vec3({ 0, -15, 1 });
		m_hostImageBuffer = nullptr;
		sphereColour.x = -1;
	}

	Scene::~Scene() {
		//checkCudaErrors1(cudaFree(m_sharedImageBuffer));
		checkCudaErrors1(cudaDeviceReset());
		delete[] m_hostImageBuffer;
		delete[] m_ImageData;
	}

	bool Scene::onInit() {
		// Allocate memory to GPU for entire application run time
		size_t numPixels = 1920 * 1080 * 2;
		size_t frame_buffer_size = numPixels * sizeof(vec3);

		checkCudaErrors1(cudaMalloc((void**) &m_deviceImageBuffer, frame_buffer_size));

		// Create imageData at constructor
		m_ImageData = new uint32_t[numPixels];

		if(!m_hostImageBuffer) {
			m_hostImageBuffer = (vec3*) malloc(frame_buffer_size);
		}

		// Pre - allocate all memory for the pointers on GPU
		checkCudaErrors1(cudaMalloc((void**) &d_camera, sizeof(camera*)));

		checkCudaErrors1(cudaMalloc((void**) &d_objList, sizeof(objectList*)));
		checkCudaErrors1(cudaMalloc((void**) &d_lightList, sizeof(lightList*)));
		checkCudaErrors1(cudaMalloc((void**) &d_world, 6 * sizeof(objectBase*)));
		checkCudaErrors1(cudaMalloc((void**) &d_lights, 6 * sizeof(lightBase*)));


		// Create the world on initialisation as the objects do not change
		// Might move later to change when updated
		create_world << <1, 1 >> > (d_objList, d_lightList, d_lights, d_world, sphereColour);
		checkCudaErrors1(cudaGetLastError());
		checkCudaErrors1(cudaDeviceSynchronize());
		m_worldChanged = false;

		// Create the camera here, update in Render if required
		create_camera << <1, 1 >> > (d_camera, m_cameraPosition, 1, 1);
		checkCudaErrors1(cudaGetLastError());
		checkCudaErrors1(cudaDeviceSynchronize());

		return true;
	}

	void Scene::OnResize(uint32_t width, uint32_t height) {
		if(m_FinalImage) {
			// No resize necessary
			if(m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) {
				return;
			}

			m_FinalImage->Resize(width, height);
		}
		else {
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}

		//delete[] m_ImageData;
		//m_ImageData = new uint32_t[width * height * 2];
	}

	// Function to actually render the image
	bool Scene::Render() {
		if(!doneInitialise) {
			onInit();
			doneInitialise = true;
		}

		//std::cerr << "RENDERING IMAGE" << std::endl;
		//auto start_time = std::chrono::high_resolution_clock::now();

		// Get dimensions of image
		int xSize = m_FinalImage->GetWidth();
		int ySize = m_FinalImage->GetHeight();

		//printf("xSize : %d, ySize : %d\n", xSize, ySize);

		// frame buffer size will control how many bytes or the size that will be copied back from
		// device memory as device memory size is constant
		size_t numPixels = xSize * 1ll * ySize;
		size_t frame_buffer_size = numPixels * sizeof(vec3);

		//checkCudaErrors1(cudaMalloc((void**) &m_deviceImageBuffer, sizeof(vec3) * 1920 * 1080));
		int tx = 32;
		int ty = 32;


		if(m_resetCamera) {
			create_camera << <1, 1 >> > (d_camera, m_cameraPosition, 1, 1);
			checkCudaErrors1(cudaGetLastError());
			checkCudaErrors1(cudaDeviceSynchronize());
			m_resetCamera = false;
		}

		// Update the camera on device
		update_camera << <1, 1 >> > (d_camera, dRight, dUp, dForward, dTheta, dPhi, xSize, ySize);
		checkCudaErrors1(cudaGetLastError());
		checkCudaErrors1(cudaDeviceSynchronize());
		dUp = 0.0f;
		dRight = 0.0f;
		dForward = 0.0f;
		dTheta = 0.0f;
		dPhi = 0.0f;

		// Update the world if it has changed
		if(m_worldChanged) {
			update_world << <1, 1 >> > (d_objList, d_lightList, d_world, sphereColour);
			checkCudaErrors1(cudaGetLastError());
			checkCudaErrors1(cudaDeviceSynchronize());
			m_worldChanged = false;
		}


		// Render the buffer
		dim3 blocks(xSize / tx + 1, ySize / ty + 1);
		dim3 threads(tx, ty);
		render << <blocks, threads >> > (m_deviceImageBuffer, xSize, ySize, d_camera, d_objList, d_lightList);
		checkCudaErrors1(cudaGetLastError());
		checkCudaErrors1(cudaDeviceSynchronize());

		// Finding max in m_deviceImageBuffer
		size_t N = numPixels * 3;

		float* h_max;
		float* d_max;
		int* d_mutex;

		// allocate memory
		h_max = (float*) malloc(sizeof(float));
		checkCudaErrors1(cudaMalloc((void**) &d_max, sizeof(float)));
		checkCudaErrors1(cudaMalloc((void**) &d_mutex, sizeof(int)));
		checkCudaErrors1(cudaMemset(d_max, 0, sizeof(float)));
		checkCudaErrors1(cudaMemset(d_mutex, 0, sizeof(float)));

		dim3 gridSize = 256;
		dim3 blockSize = 256;
		find_maximum_kernel << <gridSize, blockSize >> > (&m_deviceImageBuffer[0].x, d_max, d_mutex, N);
		checkCudaErrors1(cudaGetLastError());
		checkCudaErrors1(cudaDeviceSynchronize());

		checkCudaErrors1(cudaMemcpy(h_max, d_max, sizeof(float), cudaMemcpyDeviceToHost));


		checkCudaErrors1(cudaMemcpy(m_hostImageBuffer, m_deviceImageBuffer, frame_buffer_size, cudaMemcpyDeviceToHost));
		//checkCudaErrors1(cudaFree(m_deviceImageBuffer));

		//auto end_time = std::chrono::high_resolution_clock::now();
		//std::cerr << "\nTIME TAKEN: " << " ";
		//std::cerr << std::fixed << std::setprecision(3) << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() * 1e-3 << " s\n" << std::endl;

		for(int y = 0; y < ySize; y++) {
			for(int x = 0; x < xSize; x++) {
				size_t pixelIdx = y * xSize + x;
				//int pixelIdx = y * xSize * 3 + x * 3;
				float red = m_hostImageBuffer[pixelIdx].x / *h_max;
				float green = m_hostImageBuffer[pixelIdx].y / *h_max;
				float blue = m_hostImageBuffer[pixelIdx].z / *h_max;


				if(red < 0.0f) red = 0.0f;
				if(red > 0.99f) red = 0.99f;
				if(green < 0.0f) green = 0.0f;
				if(green > 0.99f) green = 0.99f;
				if(blue < 0.0f) blue = 0.0f;
				if(blue > 0.99f) blue = 0.99f;

				//red = clamp(red, 0.0, 0.99);
				//green = clamp(green, 0.0, 0.99);
				//blue = clamp(blue, 0.0, 0.99);

				//red = red / (red + 1.0f);
				//green = green / (green + 1.0f);
				//blue = blue / (blue + 1.0f);

				uint8_t ir = static_cast<uint8_t>((red) * 255.0f);
				uint8_t ig = static_cast<uint8_t>((green) * 255.0f);
				uint8_t ib = static_cast<uint8_t>((blue) * 256.0f);

				m_ImageData[pixelIdx] = (255 << 24) | (ib << 16) | (ig << 8) | (ir);
			}
		}

		m_FinalImage->SetData(m_ImageData);

		return true;
	}

	bool Scene::keyPressedW() {
		//m_cameraPosition.z += m_cameraStep;
		dForward += m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedA() {
		//m_cameraPosition.x -= m_cameraStep;
		dRight -= m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedS() {
		//m_cameraPosition.z -= m_cameraStep;
		dForward -= m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedD() {
		//m_cameraPosition.x += m_cameraStep;
		dRight += m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedUpArrow() {
		dPhi += m_cameraAngleStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedDownArrow() {
		dPhi -= m_cameraAngleStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedLeftArrow() {
		dTheta -= m_cameraAngleStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedRightArrow() {
		dTheta += m_cameraAngleStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedR() {
		m_resetCamera = true;
		return true;
	}

	bool Scene::keyPressedSpace() {
		dUp += m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::keyPressedLCtrl() {
		dUp -= m_cameraStep;
		m_changedState = true;
		return true;
	}

	bool Scene::mouseMoved(vec2 currPos, bool moveCamera) {
		if(!moveCamera) {
			prevMousePos = currPos;
			return false;
		}
		dTheta += (currPos.x - prevMousePos.x) / 500.0f;
		dPhi += -(currPos.y - prevMousePos.y) / 500.0f;
		//dTheta += (currPos.x) / 500.0f;
		//dPhi += -(currPos.y) / 500.0f;
		//std::cerr << currPos.x << ' ' << currPos.y << std::endl;
		prevMousePos = currPos;
		return true;
	}

	bool Scene::handleColours(vec3& sphereCol) {
		//sphereColour.x = sphereCol[0];
		//sphereColour.y = sphereCol[1];
		//sphereColour.z = sphereCol[2];
		if(sphereColour.x == sphereCol.x && sphereColour.y == sphereCol.y && sphereColour.z == sphereCol.z) {
			return false;
		}
		sphereColour = sphereCol;
		m_worldChanged = true;
		return true;
	}
};

