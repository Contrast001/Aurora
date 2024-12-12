#include "arpch.h"
#include "Application.h"


#include "Aurora/Log.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Input.h"
namespace Aurora{

#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	

	Application::Application()
	{
		AR_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());

		//��Ⱦһ��������
		//��������
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		//��������
		uint32_t indices[3] = { 0,1,2 };

		//1.���ɶ����������VAO
		m_VertexArray.reset(VertexArray::Create());
		//2.���㻺��
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices,sizeof(vertices)));

		// �趨��������ָ�룬�����Ͷ��㻺���еĶ������Բ���

		BufferLayout layout = {
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float4,"a_Color"}
		};
		//3.�����úö��㻺�岼�֣��������Ե�ֵ
		m_VertexBuffer->SetLayout(layout);
		//4.�ٸ�����������Ӷ��㻺�壬���ö��������ָ��
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);		
		//5.��������
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
		//6.����������������������
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location=0)in vec3 a_Position;
			layout(location=1)in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color=a_Color;
				gl_Position = vec4(a_Position,1.0);
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
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		//��Ⱦһ��quad
		//��������
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f, 
			 0.75f, -0.75f, 0.0f, 
			 0.75f,  0.75f, 0.0f, 
			-0.75f,  0.75f, 0.0f
		};
		//��������
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//1.���ɶ�������VAO
		m_SquareVA.reset(VertexArray::Create());
		//2.���㻺��
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices,sizeof(squareVertices)));
		// ���ö�������ָ��
		BufferLayout layout2 = {
			{ShaderDataType::Float3,"a_Position"}
		};
		//3.���ö��㻺�岼��
		squareVB->SetLayout(layout2);
		//4.��Ӷ��㻺��
		m_SquareVA->AddVertexBuffer(squareVB);
		//5.��������
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		//6.������������
		m_SquareVA->SetIndexBuffer(squareIB);
		//��ɫ������
		std::string buleShaderVertexSrc = R"(
			#version 330 core
			
			layout(location=0)in vec3 a_Position;
			
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position,1.0);
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
		m_BlueShader.reset(new Shader(buleShaderVertexSrc, buleShaderFragmentSrc));
	}


	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach(); 
	}



	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//AR_CORE_TRACE(e);

		for(auto it =m_LayerStack.end();it!=m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f,0.1f,0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			//�����ı���
			m_BlueShader->Bind();
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			//����������
			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES,m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
