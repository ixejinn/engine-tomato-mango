#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include <entt/fwd.hpp>
#include "Collision/CollisionEventFwd.h"

namespace tomato::CharacterMovement {
    void StartFalling(const TriggerExitEvent& event);
    void AfterLanding(const TriggerEnterEvent& event);
}

#endif //MANGO_CHARACTERMOVEMENT_H
