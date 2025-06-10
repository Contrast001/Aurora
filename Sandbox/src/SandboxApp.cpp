#include "Aurora.h"
#include<Aurora/Core/EntryPoint.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h" 


class ExampleLayer:public Aurora::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1280.0f/720.0f)
	{
	}

	void OnUpdate(Aurora::Timestep ts) override
	{
		
		//Update
		m_CameraController.OnUpdate(ts);

		//Render
		Aurora::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		Aurora::RenderCommand::Clear();


		Aurora::Renderer::BeginScene(m_CameraController.GetCamera());


		Aurora::Renderer::EndScene();
	}

	virtual void OnImGuiRender()override
	{
		ImGui::Begin("Setting");
		ImGui::ColorEdit3("Square Color",glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Aurora::Event& e )override
	{
		m_CameraController.OnEvent(e);
		
	}
private:
	Aurora::ShaderLibrary m_ShaderLibrary;
	Aurora::Ref<Aurora::Shader> m_Shader;
	Aurora::Ref<Aurora::VertexArray> m_VertexArray;

	Aurora::Ref<Aurora::Shader> m_FlatColorShader;
	Aurora::Ref<Aurora::VertexArray> m_SquareVA;

	Aurora::Ref<Aurora::Texture2D> m_Texture,m_AuroraLogoTexture;

	Aurora::OrthographicCameraController m_CameraController;
	

	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};

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