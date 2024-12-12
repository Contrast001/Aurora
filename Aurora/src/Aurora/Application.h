#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/LayerStack.h"
#include "Aurora/ImGui/ImGuiLayer.h"
#include "Aurora/Renderer/Shader.h"
#include "Aurora/Renderer/Buffer.h"
#include "Aurora/Renderer/VertexArray.h"


namespace Aurora {
	class AURORA_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e); 
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		static Application* s_Instance;
	};
	//在客户端定义
	Application* CreateApplication();
}
