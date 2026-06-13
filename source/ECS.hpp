#pragma once

#include "types.hpp"
#include "debug.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <typeindex>

namespace Spår
{
namespace Core
{
class App;
}

namespace ECS
{
constexpr uint32_t INVALID_ENTITY = 0;
constexpr uint32_t MAX_ENTITIES = 100'000;

class Entity;
class Scene;
class Component;

class Component
{
public:
	friend class Entity;
	friend class Scene;

	virtual ~Component() = default;

	virtual void start() {};
	virtual void update() {};

	template<typename T> requires std::is_base_of_v<Component, T>
	inline T *getSibling() const;

	inline Entity *getEntity() const;
	inline Scene *getScene() const;
	inline Core::App *getApp() const;

private:
	Entity *entity = nullptr;
};

class Entity
{
public:
	friend class Scene;

	virtual ~Entity() = default;

	uint32_t getId() const { return id; }
	bool active = false;

	virtual void start() {};
	virtual void update() {};

	template<typename T, typename ...Args> requires std::is_base_of_v<Component, T>
	T *attachComponent(Args &&...args);

	template<typename T> requires std::is_base_of_v<Component, T>
	nullptr_t detachComponent();

	template<typename T> requires std::is_base_of<Component, T>::value
	T *getComponent() const;

	inline Scene *getScene() const;
	inline Core::App *getApp() const;

private:
	uint32_t id = INVALID_ENTITY;
	Scene *scene = nullptr;
};

class Scene
{
public:
	friend class Core::App;

	virtual ~Scene() = default;

	template<typename T, typename ...Args> requires std::is_base_of_v<Entity, T>
	T *instantiate(Args&&... args)
	{
		uint32_t id = nextuint32_t++;
		if(id >= MAX_ENTITIES) [[unlikely]]
		{
			throw Debug::printError("Scene::instantiate", "Entity limit reached!");
			return nullptr;
		}

		auto entity = std::make_unique<T>(std::forward<Args>(args)...);
		entity->id = id;
		entity->active = true;
		entity->scene = this;

		T* rawPtr = entity.get();
		entities[id] = std::move(entity);
		activeEntities.push_back(id);
		return rawPtr;
	}

	template<typename T, typename ...Args> requires std::is_base_of_v<Component, T>
	T *attachComponent(Entity *entity, Args &&...args)
	{
		auto &manager = getComponentManager<T>();
		return manager.create(entity, std::forward<Args>(args)...);
	}

	template<typename T> requires std::is_base_of_v<Component, T>
	void detachComponent(Entity *entity)
	{
		getComponentManager<T>().destroy(entity->id);
	}

	template<typename T> requires std::is_base_of_v<Component, T>
	T *getComponent(Entity* entity) noexcept
	{
		auto it = componentManagers.find(typeid(T));
		if(it == componentManagers.end()) [[unlikely]]
		{
			return nullptr;
		}

		return static_cast<ComponentManager<T> *>(it->second.get())->getTyped(entity->id);
	}

	void destroyEntity(Entity *entity);
	void destroyEntity(uint32_t id);
	Entity* getEntity(uint32_t) const noexcept;

	virtual void start() {};
	virtual void update() {};

	inline Core::App *getApp() const { return app; }

protected:
	uint32_t nextuint32_t = INVALID_ENTITY + 1;
	std::unordered_map<uint32_t, std::unique_ptr<Entity> > entities;
	std::vector<uint32_t> activeEntities;

private:
	struct ComponentManagerBase
	{
		virtual ~ComponentManagerBase() = default;
		virtual void destroy(uint32_t entity) = 0;
		virtual Component* getComponent(uint32_t entity) = 0;
	};

	template<typename T> requires std::is_base_of_v<Component, T>
	struct ComponentManager : ComponentManagerBase
	{
		std::vector<std::aligned_storage_t<sizeof(T), alignof(T)>> componentStorage;
		std::unordered_map<uint32_t, size_t> entityToIndex;
		std::vector<size_t> freeIndices;

		template<typename... Args>
		T* create(Entity* entity, Args&&... args)
		{
			size_t index;
			if(freeIndices.empty())
			{
				index = componentStorage.size();
				componentStorage.emplace_back();
				entityToIndex[entity->id] = index;
			}
			else
			{
				index = freeIndices.back();
				freeIndices.pop_back();
				entityToIndex[entity->id] = index;
			}

			T* component = new (&componentStorage[index]) T(std::forward<Args>(args)...);
			component->entity = entity;
			return component;
		}

		void destroy(uint32_t entity) override
		{
			auto it = entityToIndex.find(entity);
			if(it == entityToIndex.end()) return;

			size_t index = it->second;
			T* component = reinterpret_cast<T *>(&componentStorage[index]);
			component->~T();
			freeIndices.push_back(index);
			entityToIndex.erase(it);
		}

		Component* getComponent(uint32_t entity) override
		{
			auto it = entityToIndex.find(entity);
			return it != entityToIndex.end() ? reinterpret_cast<Component *>(&componentStorage[it->second]) : nullptr;
		}

		T* getTyped(uint32_t entity) noexcept
		{
			return static_cast<T*>(getComponent(entity));
		}
	};

	std::unordered_map<std::type_index, std::unique_ptr<ComponentManagerBase>> componentManagers;

	template<typename T>
	ComponentManager<T>& getComponentManager()
	{
		std::type_index type = typeid(T);
		auto it = componentManagers.find(type);
		if(it == componentManagers.end())
		{
			auto manager = std::make_unique<ComponentManager<T>>();
			ComponentManager<T>* ptr = manager.get();
			componentManagers[type] = std::move(manager);
			return *ptr;
		}

		return *static_cast<ComponentManager<T> *>(it->second.get());
	}

	void propagateStart();
	void propagateUpdate();

	Core::App *app = nullptr;
};

template<typename T, typename ...Args> requires std::is_base_of_v<Component, T>
T* Entity::attachComponent(Args &&...args)
{
	return scene->attachComponent<T>(this, std::forward<Args>(args)...);
}

template<typename T> requires std::is_base_of_v<Component, T>
nullptr_t Entity::detachComponent()
{
	scene->detachComponent<T>(this);

	return nullptr;
}

template<typename T> requires std::is_base_of<Component, T>::value
T* Entity::getComponent() const
{
	return scene->getComponent<T>(this);
}

template<typename T> requires std::is_base_of_v<Component, T>
inline T *Component::getSibling() const
{
	return entity->getComponent<T>();
}

inline Entity* Component::getEntity() const { return entity; }
inline Scene* Component::getScene() const { return entity->getScene(); }
inline Core::App* Component::getApp() const { return entity->getScene()->getApp(); }

inline Scene* Entity::getScene() const { return scene; }
inline Core::App* Entity::getApp() const { return scene->getApp(); }
}
}