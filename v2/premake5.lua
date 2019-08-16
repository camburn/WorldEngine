workspace "WorldEngine"
    architecture "x64"
    startproject "Engine"

    configurations
    {
        "Debug",
        "Release",
    }

    newoption {
        trigger     = "opengl2",
        description = "Choose a particular 3D API for rendering"
    }
    newoption {
        trigger     = "opengl3",
        description = "Choose a particular 3D API for rendering"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "extern/glfw/include"
IncludeDir["glad2"] = "extern/glad2/include"
IncludeDir["glad3"] = "extern/glad3/include"
IncludeDir["imgui"] = "extern/imgui"
IncludeDir["glm"] = "extern/glm"

group "Dependencies"
    include "extern/"

project "Engine"
    location "./"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/")
    objdir ("build/" .. outputdir .. "/")

    pchheader "engine.hpp"
    pchsource "%{prj.name}/engine.cpp"

    files
    {
        "%{prj.name}/Engine/**.cpp",
        "%{prj.name}/Engine/**.hpp",
        "%{prj.name}/Tools/**.cpp",
        "%{prj.name}/Tools/**.hpp",
        "%{prj.name}/engine.hpp",
        "%{prj.name}/engine.cpp",
        "%{prj.name}/sandbox.cpp",
    }

    defines
    {
        "GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "extern/spdlog/include",
        "extern/tinygltf",
        "%{prj.name}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }

    links 
    { 
        "GLFW",
        "imgui",
    }

    configuration "opengl3"
        defines
        {
            "OPENGL_CORE"
        }
        files
        {
            "%{prj.name}/Platform/OpenGL/**.cpp",
            "%{prj.name}/Platform/OpenGL/**.hpp"
        }
        includedirs
        {
            "%{IncludeDir.glad3}"
        }
        links 
        {
            "glad3"
        }

    configuration "opengl2"
        defines
        {
            "OPENGL_COMPATIBILITY"
        }
        files
        {
            "%{prj.name}/Platform/OpenGL/**.cpp",
            "%{prj.name}/Platform/OpenGL/**.hpp"
        }
        includedirs
        {
            "%{IncludeDir.glad2}"
        }
        links 
        {
            "glad2"
        }

    filter "system:linux"
        staticruntime "on"
        systemversion "latest"

        files
        {
            "%{prj.name}/Platform/Linux/**.h",
            "%{prj.name}/Platform/Linux/**.cpp"
        }

        links
        {
            "Xrandr",
            "Xi",
            "GLEW",
            "GLU",
            "GL",
            "X11",
            "dl",
            "pthread"
        }

        defines
        {
            "ENGINE_PLATFORM_LINUX",
        }

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"

        files
        {
            "%{prj.name}/Platform/Windows/**.h",
            "%{prj.name}/Platform/Windows/**.cpp"
        }

        defines
        {
            "ENGINE_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        links 
        { 
            "opengl32.lib"
        }

    filter "configurations:Debug"
        defines "ENGINE_DEBUG_ENABLED"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ENGINE_RELEASE"
        runtime "Release"
        optimize "on"
