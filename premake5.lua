workspace "Aurora"    --sln文件名
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
IncludeDir["stb_image"] = "Aurora/vendor/stb_image"



group "Dependencies"
	include "Aurora/vendor/GLFW"
	include "Aurora/vendor/Glad"
	include "Aurora/vendor/imgui"
group""


project "Aurora"
	location "Aurora"
	kind "StaticLib"
	language "C++"
	cppdialect"C++17"
	-- On:代码生成的运行库选项是MTD,静态链接MSVCRT.lib库;
	-- Off:代码生成的运行库选项是MDD,动态链接MSVCRT.dll库;打包后的exe放到另一台电脑上若无这个dll会报错
	staticruntime "on"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	--预编译头
	pchheader "arpch.h"
	pchsource "Aurora/src/arpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"


	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"dwmapi.lib"
	}

	--过滤器
	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS",
			"AR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}	
		
	--不同配置的预定义不同
	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		--调试符号
		symbols "on"

	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		--优化
		optimize "on"

	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		--优化
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
	--过滤器
	filter "system:windows"
		--启用静态运行
		systemversion "latest"
		 buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS"
		}	
		
	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		--调试符号
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		--优化
		optimize "On"
		 
	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		--优化
		optimize "On"


project "Aurora-Editor"
	location "Aurora-Editor"
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
	--过滤器
	filter "system:windows"
		--启用静态运行
		systemversion "latest"
		 buildoptions { "/utf-8" }
		defines
		{
			"AR_PLATFORM_WINDOWS"
		}	
		
	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		--调试符号
		symbols "On"

	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		--优化
		optimize "On"
		 
	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		--优化
		optimize "On"