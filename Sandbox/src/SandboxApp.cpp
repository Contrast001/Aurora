#include "Aurora.h"

class ExampleLayer:public Aurora::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		AR_INFO("ExampleLayer::Update");
	}

	void OnEvent(Aurora::Event& event)override
	{
		AR_TRACE("{0}",event.ToString());
	}
};

class Sandbox :public Aurora::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Aurora::ImGuiLayer());
	}
	~Sandbox()
	{

	}

};
Aurora::Application* Aurora::CreateApplication()
{
	return new Sandbox();
}