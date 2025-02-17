#pragma once

#include "Aurora/Renderer/RendererAPI.h"

namespace Aurora {
	class OpenGLRendererAPI:public RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}