#pragma once

#include "entt/entt.hpp"

/**
 * @interface ISystem
 * @brief Abstract class which needs to be implemented by each system
 */
class ISystem {
public:
    ISystem();
    ~ISystem();

    virtual void update(entt::registry<>& registry, double deltatime) = 0;
};
