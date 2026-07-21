#ifndef MANGO_RENDER_H
#define MANGO_RENDER_H

#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"

namespace tomato
{
    struct RenderComponent
    {
        glm::vec4 color{1.f, 1.f, 1.f, 1.f};
        AssetID mesh;
        AssetID shader;
        AssetID texture;
    };
}

#endif //MANGO_RENDER_H