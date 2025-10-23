#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Aurora/Renderer/Renderer2D.h>

#include <chrono>
#include <string>

//static const uint32_t s_MapWidth = 24;
//static const char* s_MapTiles = 
//"WWWWWWWWWWWWWWWWWWWWWWWW"
//"WWWWWWWWWWWWWWWWWWWWWWWW"
//"WWWWDDDDDDDDDDDDDWWWWWW"
//"WWWWDDDWWWWWWWWWDDWWWWWW"
//"WWWDDWWWWWWDDWWWWWDDWWWW"
//"WWWDDWWWWWWWWDDWWWWDDWWW"
//"WWWDDWWDDWWWWWWWWDDWWWWW"
//"WWWDDWWWWWWWWWWDDDWWWWWW"
//"WWWWDDWWWDDWWWDDWWWWWWWW"
//"WWWWWDDDWWWWDDDWWWWWWWWW"
//"WWWWWWWDDDDDWWWWWWWWWWWW"
//"WWWWWWWWWWWWWWWWWWWWWWWW"
//"WWWWWWWWWWWWWWWWWWWWWWWW"
//"WWWWWWWWWWWWWWWWWWWWWWWW";

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

	Aurora::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Aurora::Framebuffer::Create(fbSpec);

	//m_SpriteSheet = Aurora::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	/*m_TextureStairs = Aurora::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 0,11 }, { 128,128 });
	m_TextureTree = Aurora::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2,1 }, { 128,128 }, {1,2});*/


	/*m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;*/

	/*s_TextureMap['D'] = Aurora::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6,11 }, { 128,128 });
	s_TextureMap['W'] = Aurora::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11,11 }, { 128,128 });*/


	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);
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
		m_Framebuffer->Bind();
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
	m_Framebuffer->Unbind();
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

	Aurora::Renderer2D::BeginScene(m_CameraController.GetCamera());

	/*for (uint32_t y = 0; y < m_MapHeight ; y++) 
	{
		for (uint32_t x = 0; x < m_MapWidth; x++) 
		{
			char tileType =s_MapTiles[y * m_MapWidth + x];
			Aurora::Ref<Aurora::SubTexture2D>texture;
			if (s_TextureMap.find(tileType) != s_TextureMap.end())
			{
				texture = s_TextureMap[tileType];
			}
			else 
			{
				texture = m_TextureBarrel;
			}
			Aurora::Renderer2D::DrawQuad({ x-m_MapWidth/2.0,m_MapHeight-y-m_MapHeight/ 2.0,0.5f }, { 1.0f,1.0f }, texture);


		}

	}*/
	/*Aurora::Renderer2D::DrawQuad({ 0.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureStairs);
	Aurora::Renderer2D::DrawQuad({ 1.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureBarrel);
	Aurora::Renderer2D::DrawQuad({ -1.0f,0.0f,0.5f }, { 1.0f,2.0f }, m_TextureTree);*/

	Aurora::Renderer2D::EndScene();
	

}

void Sandbox2D::OnImGuiRender()
{
	static bool dockspaceOpen = true; 
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{


			if (ImGui::MenuItem("Exit")) Aurora::Application::Get().Close();
				ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	ImGui::Begin("Settings");

	auto stats = Aurora::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	//ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 1280,720 });
	ImGui::End(); 
	ImGui::End();

	
}

void Sandbox2D::OnEvent(Aurora::Event& e)
{
	m_CameraController.OnEvent(e);
}
