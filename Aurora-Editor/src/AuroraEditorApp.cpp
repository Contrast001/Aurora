#include "Aurora.h"
#include<Aurora/Core/EntryPoint.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "EditorLayer.h" 


namespace Aurora{
	class AuroraEditor :public Application
	{
	public:
		AuroraEditor()
			:Application("Hazel Editor")
		{
			PushLayer(new EditorLayer());
		}
		~AuroraEditor()
		{

		}

	};
	Application* CreateApplication()
	{
		return new AuroraEditor();
	}
}

