#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "Camera.h"
class CameraComponent : public Component
{
	Camera _camera;
public:
	explicit CameraComponent(std::weak_ptr<GameObject> owner) : Component(owner) {}
	const auto& camera() const { return _camera; }
	auto& camera() { return _camera; }
};