#pragma once

#include "entt.hpp"

namespace Aurora{

	class Scene
	{
	public:
		Scene();
		~Scene();
	private:
		entt::registry m_Registry;
	};


}