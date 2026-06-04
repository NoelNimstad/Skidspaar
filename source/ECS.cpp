#include "ECS.hpp"

namespace Spår::ECS
{
void Entity::propagateUpdate()
{
	for(std::shared_ptr<Component> &component : components)
	{
		component->update();
	}

	update();
}

void Scene::propagateUpdate()
{
	update();

	for(std::shared_ptr<Entity> &entity : entities)
	{
		entity->propagateUpdate();
	}
}

void Entity::propagateStart()
{
	start();

	for(std::shared_ptr<Component> &component : components)
	{
		component->start();
	}
}

void Scene::propagateStart()
{
	start();

	for(std::shared_ptr<Entity> &entity : entities)
	{
		entity->propagateStart();
	}
}
}