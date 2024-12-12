#include "Aurora.h"

#include "imgui/imgui.h"


class ExampleLayer:public Aurora::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//AR_INFO("ExampleLayer::Update");
		if (Aurora::Input::IsKeyPressed(AR_KEY_TAB))
			AR_TRACE("TAB is Pressed!");
	}
	/*virtual void OnImGuiRender()override
	{
		ImGui::Begin("Test");
		ImGui::Text("1111");
		ImGui::ColorEdit4("",new float[4]);
		ImGui::End();
	}*/

	void OnEvent(Aurora::Event& event)override
	{
		if(event.GetEventType() == Aurora::EventType::KeyPressed)
		{
			Aurora::KeyPressedEvent& e = (Aurora::KeyPressedEvent&)event;
			AR_TRACE("{0}",(char)e.GetKeyCode());
		}
	}
};

class Sandbox :public Aurora::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}

};
Aurora::Application* Aurora::CreateApplication()
{
	return new Sandbox();
}