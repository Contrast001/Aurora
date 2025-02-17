#include "arpch.h"
#include "RendererCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace Aurora{

	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;
	
}