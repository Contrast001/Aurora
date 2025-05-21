#include "arpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Aurora
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	        AR_CORE_ASSERT(false, "RendererAPI:None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:          return Ref<VertexArray>();

		}

		AR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}