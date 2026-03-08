#pragma once

#include "Scene.h"

#include "entt.hpp"
namespace Aurora
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template <typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			AR_CORE_ASSERT(!HasComponent<T>(), "实体存在这个组件!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}


		template<typename T>
		T& GetComponent()
		{
			AR_CORE_ASSERT(HasComponent<T>(), "实体不存在这个组件!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}


		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			AR_CORE_ASSERT(HasComponent<T>(), "实体不存在这个组件!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}
		operator bool() const { return (uint32_t)m_EntityHandle != 0; }
	private:
		entt::entity m_EntityHandle{entt::null};
		Scene* m_Scene=nullptr;
	};
}