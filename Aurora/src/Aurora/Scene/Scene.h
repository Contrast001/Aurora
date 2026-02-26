#pragma once

#include "entt.hpp"

#include "Components.h"
#include "Aurora/Core/Timestep.h"
namespace Aurora{

	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		//Temp
		entt::registry& Reg() { return m_Registry; };

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};


}