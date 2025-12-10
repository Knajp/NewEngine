workspace "NewEngine"
   configurations { "Debug", "Release" }
   platforms {"Win64"}
   --toolset "clang"

project "NewEngine"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c", "**.cpp", "**.hpp" }

   includedirs {"vendor/include"}
   libdirs {"vendor/lib"}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter "toolset:clang"
      buildoptions { "-std=c++17", "-Wall" }
      linkoptions { "-stdlib=libc++" }

    filter "platforms:Win64"
        links {"glfw3", "glfw3_mt", "glfw3dll"}