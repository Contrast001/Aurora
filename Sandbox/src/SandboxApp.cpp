#include "Aurora.h"
#include<Aurora/Core/EntryPoint.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ExampleLayer.h"

#include "Sandbox2D.h" 




class Sandbox :public Aurora::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{

	}

};
Aurora::Application* Aurora::CreateApplication()
{
	return new Sandbox();
}