#include "arpch.h"
#include "Scene.h"

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
		struct TransformComponent 
		{
			glm::mat4 Transform;
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform)
				:Transform(transform) {}
			operator  glm::mat4& ()  { return Transform; }
			operator const glm::mat4& () const { return Transform; }
		};

		struct MeshComponent {};



		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		auto view = m_Registry.view<TransformComponent>();
		for(auto entity:view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		/*for(auto entity :group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}*/

	}

	Scene::~Scene()
	{
	}
}