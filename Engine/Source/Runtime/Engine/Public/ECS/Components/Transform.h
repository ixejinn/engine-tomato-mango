#ifndef MANGO_TRANSFORM_H
#define MANGO_TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Utils/Logger.h"

namespace tomato {
    struct TransformComponent {
        TransformComponent(
            const glm::vec3& pos = glm::vec3{0.f},
            const glm::vec3& eulerRot = glm::vec3{0.f},
            const glm::vec3& scl = glm::vec3{1.f})
                : position(pos), eulerDegree(eulerRot), scale(scl) {}

        const glm::vec3& GetPosition() const { return position; }
        void SetPosition(const glm::vec3& newPos) { position = newPos; }
        void SetPosition(const float x, const float y, const float z) { position = glm::vec3{x, y, z}; }

        const glm::vec3& GetEulerDegree() const { return eulerDegree; }
        void SetEulerDegree(const glm::vec3& newRot) {
            eulerDegree = newRot;
            rotDirty = true;
        }
        void SetEulerDegree(const float x, const float y, const float z) {
            eulerDegree = glm::vec3{x, y, z};
            rotDirty = true;
        }

        const glm::quat& GetQuaternion() const {
            if (rotDirty)
                TMT_WARN << "Quaternion is dirty. Update required.";

            return rotation;
        }

        const glm::vec3& GetScale() const { return scale; }
        void SetScale(const glm::vec3& newScl) { scale = newScl; }
        void SetScale(const float x, const float y, const float z) { scale = glm::vec3{x, y, z}; }

        const glm::mat4& GetTransformMatrix() const { return transformMatrix; }

    private:
        glm::vec3 position;
        glm::vec3 eulerDegree;
        glm::vec3 scale;

        glm::quat rotation{};
        bool rotDirty{true};

        /// Local to World.
        glm::mat4 transformMatrix{};

        friend class TransformSystem;
    };
}

#endif //MANGO_TRANSFORM_H