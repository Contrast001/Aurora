workspace "Aurora"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


IncludeDir = {}
IncludeDir["GLFW"] = "Aurora/vendor/GLFW/include"

include "Aurora/vendor/GLFW"

project "Aurora"
	location "Aurora"
	kind "SharedLib"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "arpch.h"
	pchsource "Aurora/src/arpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
		
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"dwmapi.lib"
	}
	--过滤器
	filter "system:windows"
		cppdialect "C++17"
		--启用静态运行
		staticruntime "On"
		systemversion "latest"
		buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS",
			"AR_BUILD_DLL"
		}	
		
		postbuildcommands
		{
			("{MKDIR} ../bin/" .. outputdir .. "/Sandbox"), 
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}
	filter "configurations:Debug"
		defines "AR_DEBUG"
		buildoptions "/MDd"
		--调试符号
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		buildoptions "/MD"
		--优化
		optimize "On"

	filter "configurations:Dist"
		defines "AR_DIST"
		buildoptions "/MD"
		--优化
		optimize "On"
			

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")
		
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
		
	includedirs
	{
		"Aurora/vendor/spdlog/include",
		"Aurora/src"

	}

	links
	{
		"Aurora"
	}
	--过滤器
	filter "system:windows"
		cppdialect "C++17"
		--启用静态运行
		staticruntime "On"
		systemversion "latest"
		 buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS"
		}	
		
	filter "configurations:Debug"
		defines "AR_DEBUG"
		--调试符号
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		--优化
		optimize "On"
		 
	filter "configurations:Dist"
		defines "AR_DIST"
		--优化
		optimize "On"