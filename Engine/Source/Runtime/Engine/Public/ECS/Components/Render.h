#ifndef MANGO_RENDER_H
#define MANGO_RENDER_H

#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Render/SortKey.h"

namespace tomato
{
    struct RenderComponent
    {
        glm::vec4 color{1.f, 1.f, 1.f, 1.f};
        AssetID mesh   {GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube))};
        AssetID shader {GetAssetID(Shader::PrimitiveName)};
        AssetID texture{GetAssetID(Texture::PrimitiveName)};

        RenderPriority priority{RenderPriority::Opaque};
    };
}

#endif //MANGO_RENDER_H