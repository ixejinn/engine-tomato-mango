#ifndef MANGO_TRANSFORM_H
#define MANGO_TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/fwd.hpp>
#include "ECS/Forward/TransformDirtyFwd.h"

namespace tomato
{
    struct TransformComponent
    {
        explicit TransformComponent(
            const glm::vec3& pos = glm::vec3{0.f},
            const glm::vec3& eulerRot = glm::vec3{0.f},
            const glm::vec3& scl = glm::vec3{1.f});

        glm::vec3 GetLocalPosition() const { return position; }
        glm::vec3 GetWorldPosition() const { return transformMatrix[3]; }
        void AddPosition(float x, float y, float z);
        void AddPosition(const glm::vec3& delta);
        void SetPosition(float x, float y, float z);
        void SetPosition(const glm::vec3& newPos);

        glm::vec3 GetLocalRotationDegree() const { return eulerDegree; }
        glm::vec3 GetWorldRotationDegree() const { return wEulerDegree; }
        glm::vec3 GetLocalRotationRadian() const { return glm::radians(eulerDegree); }
        glm::vec3 GetWorldRotationRadian() const { return glm::radians(wEulerDegree); }
        void AddRotationDegree(float x, float y, float z);
        void AddRotationDegree(const glm::vec3& delta);
        void SetRotationDegree(float x, float y, float z);
        void SetRotationDegree(const glm::vec3& newRot);

        glm::quat GetLocalQuaternion() const { return rotation; }
        glm::quat GetWorldQuaternion() const { return wRotation; }
        void AddQuaternion(const glm::quat& delta);
        void SetQuaternion(const glm::quat& newQuat);

        glm::vec3 GetLocalScale() const { return scale; }
        glm::vec3 GetWorldScale() const { return wScale; }
        void SetScale(float s);
        void SetScale(float x, float y, float z);
        void SetScale(const glm::vec3& newScl);

        const glm::mat4& GetTransformMatrix() const { return transformMatrix; }

    private:
        // Local
        glm::vec3 position;
        glm::vec3 eulerDegree;  // cache
        glm::quat rotation;
        glm::vec3 scale;

        // World
        glm::vec3 wEulerDegree; // cache
        glm::quat wRotation{};
        glm::vec3 wScale{};

        Transform::Dirty dirty;

        /// Local to World.
        glm::mat4 transformMatrix{};

        friend class TransformSystem;
        friend class CharacterScaleSystem;
    };
}

#endif //MANGO_TRANSFORM_H