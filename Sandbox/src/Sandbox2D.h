#pragma once

#include "Aurora.h"
class Sandbox2D:public Aurora::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;
	virtual void OnAttach()override;
	virtual void OnDetach()override;

	void OnUpdate(Aurora::Timestep ts) override;
	virtual void OnImGuiRender()override;
	void OnEvent(Aurora::Event& e)override;
private:
	Aurora::OrthographicCameraController m_CameraController;

	//Temp
	Aurora::Ref<Aurora::VertexArray> m_SquareVA;
	Aurora::Ref<Aurora::Shader> m_FlatColorShader;


	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };
};