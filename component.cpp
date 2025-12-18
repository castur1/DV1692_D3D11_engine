#include "component.hpp"

Component::Component(Entity *owner) : owner(owner) {}
Component::~Component() {}

Entity *Component::GetOwner() const {
    return this->owner;
}