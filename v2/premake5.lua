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
-- IncludeDir["Glad"] = "Hazel/vendor/Glad/include"
-- IncludeDir["ImGui"] = "Hazel/vendor/imgui"
-- IncludeDir["glm"] = "Hazel/vendor/glm"

group "Dependencies"
	include "extern/"
	-- include "Hazel/vendor/Glad"
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
        "%{prj.name}/engine.hpp",
        "%{prj.name}/engine.cpp",
        "%{prj.name}/sandbox.cpp",
		-- "%{prj.name}/vendor/glm/glm/**.hpp",
		-- "%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
	}

	includedirs
	{
        "extern/spdlog/include",
		"%{prj.name}/",
		"%{IncludeDir.glfw}",
		-- "%{IncludeDir.Glad}",
		-- "%{IncludeDir.ImGui}",
		-- "%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		-- "Glad",
		-- "ImGui",
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
            "ENGINE_PLATFORM_LINUX"
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
			"GLFW_INCLUDE_NONE"
		}

        links 
        { 
            "opengl32.lib"
        }

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"
