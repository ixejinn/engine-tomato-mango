#ifndef MANGO_TRANSFORM_H
#define MANGO_TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

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
        void AddPosition(const glm::vec3& offset);

        void SetPosition(const glm::vec3& newPos);
        void SetPosition(float x, float y, float z);

        glm::vec3 GetLocalEulerDegree() const { return eulerDegree; }
        glm::vec3 GetWorldEulerDegree() const { return glm::eulerAngles(wRotation); }
        void SetEulerDegree(const glm::vec3& newRot);
        void SetEulerDegree(float x, float y, float z);

        glm::quat GetLocalQuaternion() const { return lRotation; }
        glm::quat GetWorldQuaternion() const { return wRotation; }

        glm::vec3 GetLocalScale() const { return scale; }
        glm::vec3 GetWorldScale() const { return wScale; }
        void SetScale(const glm::vec3& newScl);
        void SetScale(float x, float y, float z);

        const glm::mat4& GetTransformMatrix() const { return transformMatrix; }

    private:
        // Local
        glm::vec3 position;
        glm::vec3 eulerDegree;
        glm::quat lRotation;
        glm::vec3 scale;

        // World
        glm::quat wRotation{};
        glm::vec3 wScale{};

        bool dirty{true};

        /// Local to World.
        glm::mat4 transformMatrix{};

        friend class TransformSystem;
    };
}

#endif //MANGO_TRANSFORM_H