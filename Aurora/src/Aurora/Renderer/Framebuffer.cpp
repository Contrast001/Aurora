#include "arpch.h"
#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Renderer.h"


namespace Aurora{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	        AR_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:       return std::make_shared<OpenGLFramebuffer>(spec);

		}

		AR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}