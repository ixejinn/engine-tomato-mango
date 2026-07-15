#ifndef MANGO_PARTICLERENDERSYSTEM_H
#define MANGO_PARTICLERENDERSYSTEM_H

#include "ECS/Systems/System.h"
#include "ECS/Forward/ParticleCompFwd.h"
#include "ECS/Forward/PhysCompFwd.h"
#include "Resource/ResourceFwd.h"

namespace tomato
{
    class ParticleRenderSystem : public System
    {
    public:
        ParticleRenderSystem();

        void Update(SimContext& simCtx) override;

    private:
        void InitializeParticles(ParticleComponent& comp, int num = 1);
        void InitializeParticles(ParticleComponent& comp, TransformComponent& trf, int num = 1);

        Mesh* mesh2D_;
        Shader* shader_;

        AssetID curTexture_;
    };
}

#endif //MANGO_PARTICLERENDERSYSTEM_H