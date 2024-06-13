-- premake5.lua
workspace "Raytracing"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Raytracing"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies.lua"
include "Raytracing/Build.lua"