#pragma once

#include "entt.hpp"

#include "Components.h"
#include "Aurora/Core/Timestep.h"
namespace Aurora{

	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name=std::string());


		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width,uint32_t heigt);
	private:
		entt::registry m_Registry;
		
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
	};


}