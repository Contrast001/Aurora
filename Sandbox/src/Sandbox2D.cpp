#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Aurora/Renderer/Renderer2D.h>

#include <chrono>
#include <string>


template<typename Fn>
class Timer 
{
public:
	Timer(const char* name,Fn&& func)
		:m_Name(name),m_Func(func),m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;
		
		float duration = (end - start) * 0.001f;

		m_Func({m_Name,duration});


	}

private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	
};


#define PROFILE_SCOPE(name)Timer timer##__LINE__(name,[&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult); })

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{

	m_CheckerboardTexture = Aurora::Texture2D::Create("assets/textures/Checkerboard.png");

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Aurora::Timestep ts)
{
	PROFILE_SCOPE("Sandbox2D::OnUpdate ");
	//Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate ");
		m_CameraController.OnUpdate(ts);
	}


	//Render
	Aurora::Renderer2D::ResetStats();
	{
		PROFILE_SCOPE("Renderer Prep ");
		Aurora::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		Aurora::RenderCommand::Clear();

	}
	
	{
		static float rotation = 0.0f;
		rotation += ts * 50.f;

		PROFILE_SCOPE("Renderer Draw ");
		Aurora::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Aurora::Renderer2D::DrawRotatedQuad({ 1.0f,0.0f }, { 0.8f,0.8f },glm::radians(- 45.0f), {0.8f,0.2f,0.3f,1.0f});
		Aurora::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
		Aurora::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.3f,0.8f,1.0f });
		Aurora::Renderer2D::DrawQuad({ 0.0f,0.0f,-0.1f }, { 20.0f,20.0f }, m_CheckerboardTexture,10.0f);
		Aurora::Renderer2D::DrawRotatedQuad({ -2.0f,0.0f,0.0f }, { 1.0f,1.0f }, glm::radians(rotation), m_CheckerboardTexture, 10.0f);
		Aurora::Renderer2D::EndScene();

		Aurora::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for(float y=-5.0f;y<5.0f;y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = {(x+0.5f)/10.0f,0.4f,(y+0.5f)/10.0f,0.7f};
				Aurora::Renderer2D::DrawQuad({ x,y }, {0.45f,0.45f},color);
			}
		}
		Aurora::Renderer2D::EndScene();

	}
	if (Aurora::Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Aurora::Input::GetMousePosition();
		auto width = Aurora::Application::Get().GetWindow().GetWidth();
		auto height = Aurora::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());

}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	
	auto stats = Aurora::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d",stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());



	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	m_ProfileResults.clear();
	ImGui::End();
}

void Sandbox2D::OnEvent(Aurora::Event& e)
{
	m_CameraController.OnEvent(e);
}
