#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Aurora/Renderer/Renderer2D.h>

#include <chrono>
#include <string>

namespace Aurora {

	template<typename Fn>
	class Timer
	{
	public:
		Timer(const char* name, Fn&& func)
			:m_Name(name), m_Func(func), m_Stopped(false)
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

			m_Func({ m_Name,duration });


		}

	private:
		const char* m_Name;
		Fn m_Func;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;

	};


#define PROFILE_SCOPE(name)Timer timer##__LINE__(name,[&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult); })

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		//Entity
		auto square = m_ActiveScene->CreateEntity("Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f,1.0f,0.0f,1.0f});

		m_SquareEntity = square;
			
		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();


		m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
		auto& cc=m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		//Resize
		if(FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x>0.0f&&m_ViewportSize.y > 0.0f&&
			(spec.Width!=m_ViewportSize.x||spec.Height!=m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)	m_ViewportSize.y);
		}
		
		//Update
		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		//Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		RenderCommand::Clear();
		
		//Update scene	
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->Unbind();

		if (Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Input::GetMousePosition();
			auto width = Application::Get().GetWindow().GetWidth();
			auto height = Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			/*m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);*/
		}

		/*m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());*/

		/*Renderer2D::BeginScene(m_CameraController.GetCamera());*/

		/*for (uint32_t y = 0; y < m_MapHeight ; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType =s_MapTiles[y * m_MapWidth + x];
				Ref<SubTexture2D>texture;
				if (s_TextureMap.find(tileType) != s_TextureMap.end())
				{
					texture = s_TextureMap[tileType];
				}
				else
				{
					texture = m_TextureBarrel;
				}
				Renderer2D::DrawQuad({ x-m_MapWidth/2.0,m_MapHeight-y-m_MapHeight/ 2.0,0.5f }, { 1.0f,1.0f }, texture);


			}

		}*/
		/*Renderer2D::DrawQuad({ 0.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureStairs);
		Renderer2D::DrawQuad({ 1.0f,0.0f,0.5f }, { 1.0f,1.0f }, m_TextureBarrel);
		Renderer2D::DrawQuad({ -1.0f,0.0f,0.5f }, { 1.0f,2.0f }, m_TextureTree);*/

		/*Renderer2D::EndScene();*/


	}

	void EditorLayer::OnImGuiRender()
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


				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		if(m_SquareEntity)
		{
			ImGui::Separator();
			auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("%s",tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
			
			ImGui::Separator();
		}
		ImGui::DragFloat3("Camera Transform",
			glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]));
		if(ImGui::Checkbox("Camera A",&m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		{
			auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused||!m_ViewportFocused);


		// 获取视口可用区域大小
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		// 安全地将ImVec2转换为glm::vec2
		glm::vec2 newViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		// 添加最小尺寸检查和浮点数比较容差
		if (newViewportSize.x > 1.0f && newViewportSize.y > 1.0f)
		{
			float widthDiff = std::abs(m_ViewportSize.x - newViewportSize.x);
			float heightDiff = std::abs(m_ViewportSize.y - newViewportSize.y);

			// 只有当尺寸变化超过1像素时才进行调整（避免频繁调整）
			if (widthDiff > 1.0f || heightDiff > 1.0f)
			{
				// 先更新视口大小，然后调整帧缓冲区
				m_ViewportSize = newViewportSize;

				// 使用四舍五入确保整数尺寸准确
				uint32_t framebufferWidth = static_cast<uint32_t>(m_ViewportSize.x + 0.5f);
				uint32_t framebufferHeight = static_cast<uint32_t>(m_ViewportSize.y + 0.5f);

				// 确保最小尺寸为1
				framebufferWidth = std::max(framebufferWidth, 1u);
				framebufferHeight = std::max(framebufferHeight, 1u);

				m_Framebuffer->Resize(framebufferWidth, framebufferHeight);
				m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			}
		}

		// 获取帧缓冲区纹理ID
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();

		// 确保使用正确的尺寸显示纹理
		ImGui::Image((void*)textureID,
			ImVec2{ m_ViewportSize.x, m_ViewportSize.y },
			ImVec2{ 0, 1 },
			ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();


	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}