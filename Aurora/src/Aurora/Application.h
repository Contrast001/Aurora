#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/LayerStack.h"
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
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
	};
	//在客户端定义
	Application* CreateApplication();
}
