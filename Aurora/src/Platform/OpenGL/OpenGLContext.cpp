#include "arpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include "glad/glad.h"

namespace Aurora{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		AR_CORE_ASSERT(windowHandle,"Window handle is null!")
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AR_CORE_ASSERT(status, "Faild to initialize Glad!");

		AR_CORE_INFO("OpenGL 信息:");
		AR_CORE_INFO("  Vendor：{0}", (const char*)glGetString(GL_VENDOR));
		AR_CORE_INFO("  显卡名：{0}", (const char*)glGetString(GL_RENDERER));
		AR_CORE_INFO("  版本：{0}", (const char*)glGetString(GL_VERSION));

	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}