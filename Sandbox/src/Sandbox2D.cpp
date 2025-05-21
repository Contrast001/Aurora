#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{


	//��Ⱦһ��quad
	//��������
	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	//��������
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	//1.���ɶ�������VAO
	m_SquareVA=Aurora::VertexArray::Create();
	//2.���㻺��
	Aurora::Ref<Aurora::VertexBuffer> squareVB;
	squareVB.reset(Aurora::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	//3.���ö��㻺�岼��
	squareVB->SetLayout({
		{Aurora::ShaderDataType::Float3,"a_Position"}
	});
	//4.��Ӷ��㻺��
	m_SquareVA->AddVertexBuffer(squareVB);
	//5.��������
	Aurora::Ref<Aurora::IndexBuffer> squareIB;
	squareIB.reset(Aurora::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	//6.������������
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Aurora::Shader::Create("assets/shaders/FlatColor.glsl");

}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Aurora::Timestep ts)
{
	//Update
	m_CameraController.OnUpdate(ts);

	//Render
	Aurora::RendererCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
	Aurora::RendererCommand::Clear();


	Aurora::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);



;
	Aurora::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));



	Aurora::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Setting");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Aurora::Event& e)
{
	m_CameraController.OnEvent(e);
}
