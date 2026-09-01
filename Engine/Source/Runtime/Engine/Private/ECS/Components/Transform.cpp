#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Components/TransformDirty.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    TransformComponent::TransformComponent(
        const glm::vec3& pos,
        const glm::vec3& eulerRot,
        const glm::vec3& scl)
    : position(pos)
    , rotation(glm::quat(glm::radians(eulerRot)))
    , scale(scl) {}

    void TransformComponent::AddPosition(const glm::vec3& delta)
    {
        position += delta;
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetPosition(const glm::vec3& newPos)
    {
        position = newPos;
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetPosition(float x, float y, float z)
    {
        position = glm::vec3{x, y, z};
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::AddRotationDegree(const glm::vec3& delta)
    {
        rotation *= glm::quat(glm::radians(delta));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetRotationDegree(const glm::vec3& newRot)
    {
        rotation = glm::quat(glm::radians(newRot));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetRotationDegree(const float x, const float y, const float z)
    {
        glm::vec3 eulerDegree{x, y, z};
        rotation = glm::quat(glm::radians(eulerDegree));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::AddQuaternion(const glm::quat& delta)
    {
        rotation *= delta;
        rotation = glm::normalize(rotation);
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetQuaternion(const glm::quat& newQuat)
    {
        rotation = newQuat;
        rotation = glm::normalize(rotation);
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetScale(const glm::vec3& newScl)
    {
        scale = newScl;
        dirty |= Transform::Dirty::Local | Transform::Dirty::Scale;
    }

    void TransformComponent::SetScale(const float x, const float y, const float z)
    {
        scale = glm::vec3{x, y, z};
        dirty |= Transform::Dirty::Local | Transform::Dirty::Scale;
    }

    void TransformComponent::SetScale(float s)
    {
        scale = glm::vec3{s, s, s};
        dirty |= Transform::Dirty::Local | Transform::Dirty::Scale;
    }
}
