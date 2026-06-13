#include "ECS.hpp"

namespace Spår::ECS
{
Entity *Scene::getEntity(uint32_t id) const noexcept
{
	auto it = entities.find(id);
	return it != entities.end() ? it->second.get() : nullptr;
}

void Scene::destroyEntity(Entity *entity)
{
	if(entity) [[likely]]
	{
		destroyEntity(entity->id);
	}
}

void Scene::destroyEntity(uint32_t id)
{
	auto it = entities.find(id);
	if(it == entities.end()) [[unlikely]]
	{
		return;
	}

	for(auto &[type, manager] : componentManagers)
	{
		manager->destroy(id);
	}

	auto activeIt = std::find(activeEntities.begin(), activeEntities.end(), id);
	if(activeIt != activeEntities.end())
	{
		std::swap(*activeIt, activeEntities.back());
		activeEntities.pop_back();
	}

	it->second->active = false;
}

void Scene::propagateStart()
{
	start();

	for(uint32_t id : activeEntities)
	{
		Entity *entity = entities[id].get();
		entity->start();
		
		for(auto& [type, manager] : componentManagers)
		{
			if(Component *component = manager->getComponent(id))
			{
				component->start();
			}
		}
	}
}

void Scene::propagateUpdate()
{
	update();

	for(uint32_t id : activeEntities)
	{
		Entity *entity = entities[id].get();
		entity->update();

		for(auto& [type, manager] : componentManagers)
		{
			if(Component *component = manager->getComponent(id))
			{
				component->update();
			}
		}
	}
}
}