workspace "NewEngine"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    toolset "clang"

    filter "platforms:x64"
        architecture "x86_64"
    filter {}

    filter "system:linux"
        includedirs {
            "./vendor/include",
            "/usr/local/include"
        }

        libdirs {
            "./vendor/lib",
            "/usr/local/lib"
        }
        links {
        "glfw3",
        "vulkan",
        "pugixml",
        "dl",
        "pthread",
        "X11",
        "Xxf86vm",
        "Xrandr",
        "Xi"
        }
        
    filter {}
    
    filter "toolset:clang"
        links "libc++"
    filter {}

project "NewEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.h", "**.c", "**.cpp", "**.hpp" }
    defines {"GLFW_INCLUDE_VULKAN", "STB_IMAGE_IMPLEMENTATION"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
