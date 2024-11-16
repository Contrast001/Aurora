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
	--������
	filter "system:windows"
		cppdialect "C++17"
		--���þ�̬����
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
		--���Է���
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		buildoptions "/MD"
		--�Ż�
		optimize "On"

	filter "configurations:Dist"
		defines "AR_DIST"
		buildoptions "/MD"
		--�Ż�
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
	--������
	filter "system:windows"
		cppdialect "C++17"
		--���þ�̬����
		staticruntime "On"
		systemversion "latest"
		 buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS"
		}	
		
	filter "configurations:Debug"
		defines "AR_DEBUG"
		--���Է���
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		--�Ż�
		optimize "On"
		 
	filter "configurations:Dist"
		defines "AR_DIST"
		--�Ż�
		optimize "On"