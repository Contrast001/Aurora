#include "arpch.h"
#include "Scene.h"

#include "Components.h"
#include "Aurora/Renderer/Renderer2D.h"

#include <glm/glm.hpp>
namespace Aurora{

	static void DoMath(const glm::mat4& transform)
	{
	}

	static void OnTransformConstruct(entt::registry& registry,entt::entity entity)
	{
	}
	Scene::Scene()
	{

		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		auto view = m_Registry.view<TransformComponent>();
		for(auto entity:view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		

	}

	Scene::~Scene()
	{
	}
	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}
	void Scene::OnUpdate(Timestep ts)
	{

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
}