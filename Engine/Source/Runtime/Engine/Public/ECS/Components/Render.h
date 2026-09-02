#ifndef MANGO_RENDER_H
#define MANGO_RENDER_H

#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"
#include "Render/RenderType.h"

namespace tomato
{
    struct RenderComponent
    {
        glm::vec4 color{1.f, 1.f, 1.f, 1.f};
        AssetID shader{0};
        AssetID texture{0};
        AssetID mesh{0};

        RenderType type{RenderType::Object};
    };

    struct WorldObjectTag {};
}

#endif //MANGO_RENDER_H