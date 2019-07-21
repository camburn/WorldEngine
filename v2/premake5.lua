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

group ""

project "Engine"
	location "./"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "./")
	objdir ("build/" .. outputdir .. "./")

	pchheader "engine.hpp"
	pchsource "%{prj.name}/engine.cpp"

	files
	{
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.hpp",
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
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			-- "HZ_PLATFORM_WINDOWS",
			-- "HZ_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"
