#ifndef CUDA_INCLUDE_H
#define CUDA_INCLUDE_H

#include <iostream>
#include <cuda.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"



#ifdef USE_GLM
#define GLM_FORCE_CUDA
#include "glm/glm.hpp"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
#else
#include "math/ext/helper_math.h"


using vec2 = float2;
using vec3 = float3;
using vec4 = float4;

#endif

constexpr float PI = 3.141592653589793238f;
constexpr float PI_2 = PI / 2;


#endif