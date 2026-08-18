#ifndef MANGO_EDITORCAMERASYSTEM_H
#define MANGO_EDITORCAMERASYSTEM_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "ECS/Systems/System.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    class EditorCameraSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        bool isFreeLooking_{false};

        glm::vec2 preCursorPos;
        glm::vec3 initCameraEulerDegree;
    };
}

#endif //MANGO_EDITORCAMERASYSTEM_H