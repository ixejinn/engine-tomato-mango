#ifndef MANGO_CAMERA_H
#define MANGO_CAMERA_H

#include <glm/mat4x4.hpp>
#include "Serialization/Json.h"

namespace tomato {
    enum ProjectionMode {
        Perspective,
        Orthogonal
    };

    struct CameraComponent {
        CameraComponent(
            const ProjectionMode m = ProjectionMode::Perspective,
            const float degree = 45.f,
            const float zNear = 0.1f,
            const float zFar = 100.f)
                : mode(m), degree(degree), zNear(zNear), zFar(zFar), viewProjMat(1.f) {}

        ProjectionMode mode;

        float degree;
        float zNear;
        float zFar;
        glm::mat4 viewProjMat;

        bool dirty{ true };
    };

    struct MainCameraTag {};

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ProjectionMode,
        {
            { ProjectionMode::Perspective, "Perspective"},
            { ProjectionMode::Orthogonal, "Orthogonal"}
        }
    )
}

#endif //MANGO_CAMERA_H