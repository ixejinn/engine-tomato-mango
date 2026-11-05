#ifndef MANGO_STATE_H
#define MANGO_STATE_H

#include <array>
#include <memory>
#include <unordered_map>
#include <entt/entity/registry.hpp>
#include "State/StateFwd.h"
#include "Services/ServiceFwd.h"
#include "Resource/ResourceFwd.h"
#include "Serialization/UUID.h"
#include "Engine/TomatoFwd.h"
#include "Collision/Layer/CollisionLayerMatrix.h"
#include "Utils/PassKey.h"

namespace tomato
{
    class State
    {
    public:
        explicit State(Engine& engine);
        virtual ~State() = default;

        virtual void Init()   {};
        virtual void Update() {};
        virtual void Exit()   {};

        virtual void CreateDefaultEntities() {}

        entt::registry& GetRegistry() { return registry_; }

        Window& GetWindow();

        CollisionLayerMatrix& GetCollisionLayerMatrix();
        PlayerInputTimelines& GetPlayerInputTimelines() { return playerInputs_; }

        void SetPlayerInput(uint32_t tick, IntentState input, int playerID);

    protected:
        Engine& engine_;

        entt::registry registry_;
        PlayerInputTimelines playerInputs_;

        PassKey<State> key_;
    };
}

#endif //MANGO_STATE_H