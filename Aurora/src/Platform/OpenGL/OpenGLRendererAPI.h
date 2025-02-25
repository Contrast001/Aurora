#pragma once

#include "Aurora/Renderer/RendererAPI.h"

namespace Aurora {
	class OpenGLRendererAPI:public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}