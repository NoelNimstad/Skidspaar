#pragma once

#include "types.hpp"

#include <vector>
#include <memory>
#include <type_traits>

namespace Spår
{
namespace Core
{
// Forward declaration
class App;
}

namespace ECS
{
// Forward declarations
class Entity;
class Scene;

class Component
{
public:
	friend class Entity;

	virtual ~Component() = default;

	virtual void start() {};
	virtual void update() {};

	Entity *getEntity() { return entity; }

private:
	Entity *entity = nullptr;
};

class Entity
{
public:
	friend class Scene;

	virtual ~Entity() = default;

	virtual void start() {};
	virtual void update() {};

	template<typename T, typename ...Args> requires std::is_base_of<Component, T>::value
	[[maybe_unused]] std::shared_ptr<T> attachComponent(Args &&...args)
	{
		auto c = std::make_shared<T>(std::forward<Args>(args)...);
		c->entity = this;

		components.push_back(c);
		return c;
	}

	Scene *getScene() { return scene; }

private:
	std::vector<std::shared_ptr<Component> > components;

	void propagateStart();
	void propagateUpdate();

	Scene *scene = nullptr;
};

class Scene
{
public:
	friend class Core::App;

	virtual ~Scene() = default;

	template<typename T, typename ...Args> requires std::is_base_of<Entity, T>::value
	[[maybe_unused]] std::shared_ptr<T> instantiate(Args &&...args)
	{
		auto e = std::make_shared<T>(std::forward<Args>(args)...);
		e->scene = this;

		entities.push_back(e);
		return e;
	}

	virtual void start() {};
	virtual void update() {};

	Core::App *getApp() { return app; }

private:
	std::vector<std::shared_ptr<Entity> > entities;

	void propagateStart();
	void propagateUpdate();

	Core::App *app = nullptr;
};
}
}