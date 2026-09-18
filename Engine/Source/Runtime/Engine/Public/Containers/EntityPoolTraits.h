#ifndef MANGO_ENTITYPOOLTRAITS_H
#define MANGO_ENTITYPOOLTRAITS_H

#include <concepts>
#include <entt/fwd.hpp>

namespace tomato
{
    /*
    * EntityPool Traits
    *
    * Required:
    *
    *   static void Assemble(
    *       entt::registry&,
    *       entt::entity);
    *
    *   static void Reset(
    *       entt::registry&,
    *       entt::entity,
    *       ...);
    *
    *   static bool Deactivate(
    *       entt::registry&,
    *       entt::entity);
    *
    * Assemble:
    *   Called once when the pool creates the entity.
    *
    * Reset:
    *   Called every time the entity is acquired.
    *
    * Deactivate:
    *   Called when the entity is released.
    *   Return false to reject the release.
    */

    // Detect whether "Deactivate" exists at compile time.
    template<typename Traits>
    concept HasDeactivate = requires(entt::registry & registry_, entt::entity entity)
    {
        { Traits::Deactivate(registry_, entity) };
    };

    template<typename Traits>
    concept EntityPoolTraits = requires(entt::registry & registry_, entt::entity entity)
    {
        { Traits::Assemble(registry_, entity) } -> std::same_as<void>;
    };
}

#endif // !MANGO_ENTITYPOOLTRAITS_H
