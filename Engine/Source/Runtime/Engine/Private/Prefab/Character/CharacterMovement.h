#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include <entt/fwd.hpp>
#include "Collision/CollisionEventFwd.h"

namespace tomato::CharacterMovement {
    void StartFalling(const TriggerExitEvent& event, entt::entity e);
    void AfterLanding(const TriggerEnterEvent& event, entt::entity e);
}

#endif //MANGO_CHARACTERMOVEMENT_H
