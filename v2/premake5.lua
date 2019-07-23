workspace "WorldEngine"
	architecture "x64"
	startproject "Engine"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "extern/glfw/include"
IncludeDir["glad"] = "extern/glad/include"
IncludeDir["imgui"] = "extern/imgui"
IncludeDir["glm"] = "extern/glm/glm"

group "Dependencies"
	include "extern/"
	-- include "extern/vendor/glad"
	-- include "Hazel/vendor/imgui"


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
        "%{prj.name}/Platform/OpenGL/**.cpp",
        "%{prj.name}/Platform/OpenGL/**.hpp",
        "%{prj.name}/engine.hpp",
        "%{prj.name}/engine.cpp",
        "%{prj.name}/sandbox.cpp",
		-- "%{prj.name}/vendor/glm/glm/**.hpp",
		-- "%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
        "extern/spdlog/include",
		"%{prj.name}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"glad",
		"imgui",
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
            "OPENGL_COMPATIBILITY"
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
