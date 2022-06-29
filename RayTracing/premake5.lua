require("premake5-cuda")

architecture "x64"

project "RayTracing"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.hpp", "src/**.cpp", "src/**.cuh", "src/**.cu" }

   includedirs
   {
      "../Walnut/vendor/imgui",
      "../Walnut/vendor/glfw/include",
      "../Walnut/vendor/glm",

      "../Walnut/Walnut/src",

      "%{IncludeDir.VulkanSDK}",
   }

   links
   {
       "Walnut"
   }


   -- Add build customisation for CUDA
   buildcustomizations "BuildCustomizations/CUDA 11.7"

   -- CUDA properties
   cudaMaxRegCount "32"
   cudaCodeGeneration { "compute_75,sm_75" } -- Specifies names of NVIDIA GPUs to generate code for, change as required


   -- Additional arguments to pass to NVCC. std c++17 is required for Walnut.
   cudaCompilerOptions { "--std c++17" }  

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "WL_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "WL_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"