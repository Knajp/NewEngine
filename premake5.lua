workspace "NewEngine"
    configurations { "Debug", "Release" }
    platforms { "x64" }

    filter "platforms:x64"
        architecture "x86_64"
    filter {}

    filter "system:linux"
        toolset "clang"

        
    filter {}

project "NewEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.h", "**.c", "**.cpp", "**.hpp" }

    includedirs {
        "vendor/include",
        "/usr/local/include"
    }

    libdirs {
        "vendor/lib",
        "/usr/local/lib"
    }

    links {
        "glfw3",
        "vulkan",
        "dl",
        "pthread",
        "X11",
        "Xxf86vm",
        "Xrandr",
        "Xi"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
