#include "component.hpp"

Component::Component(Entity *owner, bool isActive) : owner(owner), isActive(isActive) {}
Component::~Component() {}

Entity *Component::GetOwner() const {
    return this->owner;
}