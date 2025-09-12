#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() :Layer("Example"), m_CameraController(1280.0f / 720.0f)
{
}

void ExampleLayer::OnUpdate(Aurora::Timestep ts)
{
	//Update
	m_CameraController.OnUpdate(ts);

	//Render
	Aurora::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
	Aurora::RenderCommand::Clear();


	Aurora::Renderer::BeginScene(m_CameraController.GetCamera());


	Aurora::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Setting");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(Aurora::Event& e)
{
	m_CameraController.OnEvent(e);
}
