#pragma once
#include <type_traits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <concepts>
#include <stdexcept>
class GameObject;
class Component
{
public:
	explicit Component(std::weak_ptr<GameObject> owner) : owner(owner) {}
	virtual ~Component() = default;
	std::shared_ptr<GameObject> GetOwner() const { return owner.lock(); }
	std::string GetName() const { return name; }
	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool enabled) { this->enabled = enabled; }
	void SetName(const std::string& name) { this->name = name; }
protected:
	std::weak_ptr<GameObject> owner;
	std::string name;
	bool enabled = true;
	bool IsOwnerValid() const { return !owner.expired(); }
};