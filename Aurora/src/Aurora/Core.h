#pragma once

#ifdef AR_PLATFORM_WINDOWS
	#if AR_DYNAMIC_LINK
		#ifdef AR_BUILD_DLL
			#define AURORA_API __declspec(dllexport)
		#else
			#define AURORA_API __declspec(dllimport)
		#endif
	#else
		#define AURORA_API 
#endif
#else
	#error Aurora only supports Windows!
#endif


#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS
#endif
//（如果x表示错误则语句运行，{0}占位的"__VA_ARGS__"代表"..."所输入的语句）
#ifdef AR_ENABLE_ASSERTS
	#define AR_ASSERT(x, ...) \
		{ if(!(x)) { AR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

	#define AR_CORE_ASSERT(x, ...) \
		{ if(!(x)) { AR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define AR_ASSERT(x, ...)
#define AR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)

#define AR_BIND_EVENT_FN(fn)std::bind(&fn,this,std::placeholders::_1)