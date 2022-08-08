# RayTracing-CUDA
Ray Tracer written in C++. Uses CUDA.

---
## Requirements

This application was built using [Walnut](https://github.com/TheCherno/Walnut) as a template. Has the same requirements.
- [Visual Studio 2022](https://visualstudio.com/) (2022 is preferred, not strictly required. Requires "desktop development with C++" workload)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (most recent version)
- [CUDA toolkit](https://docs.nvidia.com/cuda/cuda-installation-guide-microsoft-windows/index.html)  for Windows (Tested on 11.7)

## Setup

1. Clone the repository recursively `git clone --recursive https://github.com/furmarie/RayTracing-CUDA.git`
2. Run `scripts/setup.bat`. This will create all VS2022 solutions and files.
3. Open `RayTracing.sln` in Visual Studio, in the `Solution Explorer` right-click on `scene.cu` and `scene.cuh` select `Properties` then change `Item Type` to `CUDA C/C++`. These files will be compiled by `NVCC`. 

Find the compute capability of your GPU [here](https://developer.nvidia.com/cuda-gpus) and add it to `Code Generation` in the `CUDA C/C++ / Device`  tab
in `RayTracing` project properties. This step is necessary if the project does not get built or does not run. (Requires compute capability 2.x and above)

## Building and running
Select `Release` configuration in Visual Studio(debug messes things up), then hit F5. 


## Credits
Premake5 for CUDA toolkit integration - https://github.com/theComputeKid/premake5-cuda
