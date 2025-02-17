#include "Aurora.h"

#include "imgui/imgui.h"


class ExampleLayer:public Aurora::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Aurora::VertexArray::Create());

		//渲染一个三角形
		//顶点数据
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		//索引数据
		uint32_t indices[3] = { 0,1,2 };

		//1.生成顶点数组对象VAO
		m_VertexArray.reset(Aurora::VertexArray::Create());
		//2.顶点缓冲
		std::shared_ptr<Aurora::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Aurora::VertexBuffer::Create(vertices, sizeof(vertices)));

		// 设定顶点属性指针，来解释顶点缓冲中的顶点属性布局

		Aurora::BufferLayout layout = {
			{Aurora::ShaderDataType::Float3,"a_Position"},
			{Aurora::ShaderDataType::Float4,"a_Color"}
		};
		//3.先设置好顶点缓冲布局，计算属性的值
		m_VertexBuffer->SetLayout(layout);
		//4.再给顶点数组添加顶点缓冲，设置顶点的属性指针
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		//5.索引缓冲
		std::shared_ptr<Aurora::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Aurora::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		//6.给顶点数组设置索引缓冲
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location=0)in vec3 a_Position;
			layout(location=1)in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color=a_Color;
				gl_Position =u_ViewProjection * vec4(a_Position,1.0);
			} 
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location=0)out vec4 color;
			
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			} 
		)";
		m_Shader.reset(new Aurora::Shader(vertexSrc, fragmentSrc));

		//渲染一个quad
		//顶点数据
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		//索引数据
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//1.生成顶点数组VAO
		m_SquareVA.reset(Aurora::VertexArray::Create());
		//2.顶点缓冲
		std::shared_ptr<Aurora::VertexBuffer> squareVB;
		squareVB.reset(Aurora::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		// 设置顶点属性指针
		Aurora::BufferLayout layout2 = {
			{Aurora::ShaderDataType::Float3,"a_Position"}
		};
		//3.设置顶点缓冲布局
		squareVB->SetLayout(layout2);
		//4.添加顶点缓冲
		m_SquareVA->AddVertexBuffer(squareVB);
		//5.索引缓冲
		std::shared_ptr<Aurora::IndexBuffer> squareIB;
		squareIB.reset(Aurora::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		//6.设置索引缓冲
		m_SquareVA->SetIndexBuffer(squareIB);
		//着色器代码
		std::string buleShaderVertexSrc = R"(
			#version 330 core
			
			layout(location=0)in vec3 a_Position;
			

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position =u_ViewProjection*vec4(a_Position,1.0);
			} 
		)";

		std::string buleShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location=0)out vec4 color;
			
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.2,0.3,0.8,1.0);
			} 
		)";
		m_BlueShader.reset(new Aurora::Shader(buleShaderVertexSrc, buleShaderFragmentSrc));
	}

	void OnUpdate(Aurora::Timestep ts) override
	{
		if (Aurora::Input::IsKeyPressed(AR_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed*ts;

		else if (Aurora::Input::IsKeyPressed(AR_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Aurora::Input::IsKeyPressed(AR_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;

		else if (Aurora::Input::IsKeyPressed(AR_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Aurora::Input::IsKeyPressed(AR_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;

		else if (Aurora::Input::IsKeyPressed(AR_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Aurora::RendererCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		Aurora::RendererCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		Aurora::Renderer::BeginScene(m_Camera);
		Aurora::Renderer::Submit(m_BlueShader, m_SquareVA);

		Aurora::Renderer::Submit(m_Shader, m_VertexArray);

		Aurora::Renderer::EndScene();
	}

	virtual void OnImGuiRender()override
	{

	}

	void OnEvent(Aurora::Event& event)override
	{
	}
private:
	std::shared_ptr<Aurora::Shader> m_Shader;
	std::shared_ptr<Aurora::VertexArray> m_VertexArray;

	std::shared_ptr<Aurora::Shader> m_BlueShader;
	std::shared_ptr<Aurora::VertexArray> m_SquareVA;

	Aurora::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 30.0f;

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