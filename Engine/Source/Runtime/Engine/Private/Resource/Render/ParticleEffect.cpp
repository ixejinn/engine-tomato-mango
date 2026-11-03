#include "Resource/Render/ParticleEffect.h"
#include "Resource/AssetRegistry.h"
#include "Serialization/JsonIO.h"

namespace tomato
{
    ParticleEffect::ParticleEffect(const std::filesystem::path& path)
    {
        auto data = Serialization::LoadJsonData(path.string().c_str());
        sets_ = data.get<ParticleEmitterSettings>();
    }

    void ParticleEffect::Create(const std::filesystem::path& path)
    {
        std::unique_ptr<ParticleEffect> ptr{new ParticleEffect(path)};
        AssetRegistry<ParticleEffect>::GetInstance().Register(path.string(), std::move(ptr));
    }
}