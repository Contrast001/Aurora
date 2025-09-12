#pragma once
#include <Aurora.h>

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
class ExampleLayer :public Aurora::Layer
{
public:
	ExampleLayer();
		

	void OnUpdate(Aurora::Timestep ts) override;

	virtual void OnImGuiRender()override; 

	void OnEvent(Aurora::Event& e)override;
	
private:
	Aurora::ShaderLibrary m_ShaderLibrary;
	Aurora::Ref<Aurora::Shader> m_Shader;
	Aurora::Ref<Aurora::VertexArray> m_VertexArray;

	Aurora::Ref<Aurora::Shader> m_FlatColorShader;
	Aurora::Ref<Aurora::VertexArray> m_SquareVA;

	Aurora::Ref<Aurora::Texture2D> m_Texture, m_AuroraLogoTexture;

	Aurora::OrthographicCameraController m_CameraController;


	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};