#pragma once

#include "Base.h"

#include "Aurora/Events/Event.h"
#include "Window.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/Core/LayerStack.h"
#include "Aurora/ImGui/ImGuiLayer.h"
#include "Aurora/Core/Timestep.h"


namespace Aurora {
	class Application
	{
	public:
		Application(const std::string& name = "Aurora App");
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e); 
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime=0.0f;
		
	private:
		static Application* s_Instance;
	};
	//在客户端定义
	Application* CreateApplication();
}
