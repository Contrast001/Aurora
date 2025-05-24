#include "arpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace Aurora{

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
	
}