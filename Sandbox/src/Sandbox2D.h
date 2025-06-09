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

	Aurora::Ref<Aurora::Texture2D> m_CheckerboardTexture;

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;
	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };
};