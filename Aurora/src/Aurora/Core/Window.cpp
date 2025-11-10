#include "arpch.h"
#include "Aurora/Core/Window.h"

#ifdef AR_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Aurora{

	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef AR_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);	
	#else
		AR_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}
}