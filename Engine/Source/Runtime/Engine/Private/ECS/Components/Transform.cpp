#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "ECS/Components/Transform.h"
#include "Utils/Logger.h"

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
        dirty = true;
    }

    void TransformComponent::SetPosition(const glm::vec3& newPos)
    {
        position = newPos;
        dirty = true;
    }

    void TransformComponent::SetPosition(float x, float y, float z)
    {
        position = glm::vec3{x, y, z};
        dirty = true;
    }

    void TransformComponent::AddRotationDegree(const glm::vec3& delta)
    {
        rotation *= glm::quat(glm::radians(delta));
        dirty = true;
    }

    void TransformComponent::SetRotationDegree(const glm::vec3& newRot)
    {
        rotation = glm::quat(glm::radians(newRot));
        dirty = true;
    }

    void TransformComponent::SetRotationDegree(const float x, const float y, const float z)
    {
        glm::vec3 eulerDegree{x, y, z};
        rotation = glm::quat(glm::radians(eulerDegree));
        dirty = true;
    }

    void TransformComponent::AddQuaternion(const glm::quat& delta)
    {
        rotation *= delta;
        rotation = glm::normalize(rotation);
        dirty = true;
    }

    void TransformComponent::SetQuaternion(const glm::quat& newQuat)
    {
        rotation = newQuat;
        rotation = glm::normalize(rotation);
        dirty = true;
    }

    void TransformComponent::SetScale(const glm::vec3& newScl)
    {
        scale = newScl;
        dirty = true;
    }

    void TransformComponent::SetScale(const float x, const float y, const float z)
    {
        scale = glm::vec3{x, y, z};
        dirty = true;
    }

    void TransformComponent::SetScale(float s)
    {
        scale = glm::vec3{s, s, s};
        dirty = true;
    }
}
