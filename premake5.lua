workspace "Aurora"    --sln�ļ���
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

startproject "Sandbox"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"



IncludeDir = {}
IncludeDir["GLFW"] = "Aurora/vendor/GLFW/include"
IncludeDir["Glad"] = "Aurora/vendor/Glad/include"
IncludeDir["ImGui"] = "Aurora/vendor/imgui"
IncludeDir["glm"] = "Aurora/vendor/glm"



include "Aurora/vendor/GLFW"
include "Aurora/vendor/Glad"
include "Aurora/vendor/imgui"



project "Aurora"
	location "Aurora"
	kind "StaticLib"
	language "C++"
	cppdialect"C++17"
	-- On:�������ɵ����п�ѡ����MTD,��̬����MSVCRT.lib��;
	-- Off:�������ɵ����п�ѡ����MDD,��̬����MSVCRT.dll��;������exe�ŵ���һ̨�������������dll�ᱨ��
	staticruntime "on"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	--Ԥ����ͷ
	pchheader "arpch.h"
	pchsource "Aurora/src/arpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}
	defines{
		"_CRT_SECURE_NO_WARNINGS"
	}
		
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"

	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"dwmapi.lib"
	}

	--������
	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS",
			"AR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}	
		
	--��ͬ���õ�Ԥ���岻ͬ
	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		--���Է���
		symbols "on"

	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		--�Ż�
		optimize "on"

	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		--�Ż�
		optimize "on"
			

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect"C++17"
	staticruntime "On"
	buildoptions { "/utf-8" }

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
		"Aurora/src",
		"Aurora/vendor",
		"%{IncludeDir.glm}"

	}

	links
	{
		"Aurora"
	}
	--������
	filter "system:windows"
		--���þ�̬����
		systemversion "latest"
		 buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS"
		}	
		
	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		--���Է���
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		--�Ż�
		optimize "On"
		 
	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		--�Ż�
		optimize "On"