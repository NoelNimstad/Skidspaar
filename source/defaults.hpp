#pragma once

#include <string>

#include "ECS.hpp"
#include "types.hpp"

namespace Spår::ECS::Defaults
{
namespace Components
{
class Transform : public Component
{
public:
	Transform() = default;
	Transform(Utility::Types::Vector2 position) : position(position) {}
	Transform(Utility::Types::Vector2 position, Utility::Types::float32_t rotation)
		: position(position), rotation(rotation) {}

	Utility::Types::Vector2 position;
	Utility::Types::float32_t rotation = 0.0f;
	Utility::Types::Vector2 scale = Utility::Types::Vector2(1.0f, 1.0f);
};

class RigidBody : public Component
{
public:
	RigidBody(Transform *transform)
	{
		if(nullptr == transform)
		{
			throw Debug::printError("RigidBody::RigidBody", "Invalid transform assigned to RigidBody!");
		}
	}

protected:
	Transform *transform;
};
}

namespace Entities
{
class StandardEntity : public Entity
{
public:
	StandardEntity()
	{
		transform = attachComponent<Components::Transform>();
		rb = attachComponent<Components::RigidBody>(transform);
	}

protected:
	Components::Transform *transform;
	Components::RigidBody *rb;
};
}
}