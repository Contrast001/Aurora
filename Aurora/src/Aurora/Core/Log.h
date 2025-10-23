#pragma once


#include"Base.h"
#include"spdlog/spdlog.h"
#include"spdlog/fmt/ostr.h"

namespace Aurora {
	class AURORA_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Coreºê
#define AR_CORE_TRACE(...)		::Aurora::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AR_CORE_INFO(...)		::Aurora::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AR_CORE_WARN(...)		::Aurora::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AR_CORE_ERROR(...)		::Aurora::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AR_CORE_FATAL(...)		::Aurora::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Clientºê
#define AR_TRACE(...)			::Aurora::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AR_INFO(...)			::Aurora::Log::GetClientLogger()->info(__VA_ARGS__)
#define AR_WARN(...)			::Aurora::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AR_ERROR(...)			::Aurora::Log::GetClientLogger()->error(__VA_ARGS__)
#define AR_FATAL(...)			::Aurora::Log::GetClientLogger()->fatal(__VA_ARGS__)