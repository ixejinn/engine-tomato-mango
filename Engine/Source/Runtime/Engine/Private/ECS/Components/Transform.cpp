#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Utils/Logger.h"
#include "ECS/Components/Transform.h"

namespace tomato {
    TransformComponent::TransformComponent(
        const glm::vec3& pos,
        const glm::vec3& eulerRot,
        const glm::vec3& scl)
            : position(pos), eulerDegree(eulerRot), scale(scl) {
        lRotation = glm::quat(glm::radians(eulerDegree));
    }

    void TransformComponent::AddPosition(const glm::vec3& offset)  {
        position += offset;
        dirty = true;
    }

    void TransformComponent::SetPosition(const glm::vec3& newPos) {
        position = newPos;
        dirty = true;
    }

    void TransformComponent::SetPosition(float x, float y, float z) {
        position = glm::vec3{x, y, z};
        dirty = true;
    }

    void TransformComponent::SetEulerDegree(const glm::vec3& newRot) {
        eulerDegree = newRot;
        lRotation = glm::quat(glm::radians(eulerDegree));
        dirty = true;
    }

    void TransformComponent::SetEulerDegree(const float x, const float y, const float z) {
        eulerDegree = glm::vec3{x, y, z};
        lRotation = glm::quat(glm::radians(eulerDegree));
        dirty = true;
    }

    void TransformComponent::SetScale(const glm::vec3& newScl) {
        scale = newScl;
        dirty = true;
    }

    void TransformComponent::SetScale(const float x, const float y, const float z) {
        scale = glm::vec3{x, y, z};
        dirty = true;
    }
}