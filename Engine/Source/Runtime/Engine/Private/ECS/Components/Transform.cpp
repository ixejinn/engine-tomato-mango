#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Components/TransformDirty.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "Math/Angle.h"

namespace tomato
{
    TransformComponent::TransformComponent(
        const glm::vec3& pos,
        const glm::vec3& eulerRot,
        const glm::vec3& scl)
    : position(pos)
    , eulerDegree(eulerRot)
    , rotation(glm::quat(glm::radians(eulerRot)))
    , scale(scl), dirty(Transform::Dirty::Local | Transform::Dirty::Scale) {}

    void TransformComponent::AddPosition(float x, float y, float z)
    {
        AddPosition({x, y, z});
    }

    void TransformComponent::AddPosition(const glm::vec3& delta)
    {
        position += delta;
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetPosition(float x, float y, float z)
    {
        SetPosition({x, y, z});
    }

    void TransformComponent::SetPosition(const glm::vec3& newPos)
    {
        position = newPos;
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::AddRotationDegree(float x, float y, float z)
    {
        AddRotationDegree({x, y, z});
    }

    void TransformComponent::AddRotationDegree(const glm::vec3& delta)
    {
        eulerDegree += delta;
        rotation = glm::quat(glm::radians(eulerDegree));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetRotationDegree(const float x, const float y, const float z)
    {
        SetRotationDegree({x, y, z});
    }

    void TransformComponent::SetRotationDegree(const glm::vec3& newRot)
    {
        eulerDegree = newRot;
        rotation = glm::quat(glm::radians(eulerDegree));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::AddQuaternion(const glm::quat& delta)
    {
        eulerDegree += glm::degrees(glm::eulerAngles(glm::normalize(delta)));
        rotation = glm::quat(glm::radians(eulerDegree));
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetQuaternion(const glm::quat& newQuat)
    {
        rotation = glm::normalize(newQuat);
        eulerDegree = GetClosestEulerDegree(rotation, eulerDegree);
        dirty |= Transform::Dirty::Local;
    }

    void TransformComponent::SetScale(const glm::vec3& newScl)
    {
        scale = newScl;
        dirty |= Transform::Dirty::Local | Transform::Dirty::Scale;
    }

    void TransformComponent::SetScale(const float x, const float y, const float z)
    {
        SetScale({x, y, z});
    }

    void TransformComponent::SetScale(float s)
    {
        SetScale({s, s, s});
    }
}
