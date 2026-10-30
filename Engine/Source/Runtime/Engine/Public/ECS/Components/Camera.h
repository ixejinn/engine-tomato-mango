#ifndef MANGO_CAMERA_H
#define MANGO_CAMERA_H

#include <glm/mat4x4.hpp>
#include "Serialization/Json.h"

namespace tomato
{
    enum ProjectionMode
    {
        Perspective,
        Orthogonal
    };

    struct CameraComponent
    {
        ProjectionMode mode{Perspective};

        float degree{45};
        float zNear{0.1};
        float zFar{100};

        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 back;

        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjMat;

        bool dirty{true};
    };

    struct MainCameraTag {};

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ProjectionMode,
        {
            {ProjectionMode::Perspective, "Perspective"},
            {ProjectionMode::Orthogonal, "Orthogonal"}
        }
    )
}

#endif //MANGO_CAMERA_H