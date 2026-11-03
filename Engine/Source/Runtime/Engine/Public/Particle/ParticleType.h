#ifndef MANGO_PARTICLETYPE_H
#define MANGO_PARTICLETYPE_H

#include "Serialization/Json.h"

namespace tomato
{
    //// Emitter shape
    // 파티클이 생성될 때 초기 속도의 방향을 결정
#define TMT_PARTICLE_EMITTER_SHAPE_LIST(X)  \
        X(Cone,   "Cone")                   \
        X(Sphere, "Sphere")                 \
        X(Circle, "Circle")

    enum class ParticleEmitterShape
    {
#define X(Enum, Display) Enum,
        TMT_PARTICLE_EMITTER_SHAPE_LIST(X)
#undef X
    };

    struct ParticleEmitterShapeMeta
    {
        ParticleEmitterShape shape;
        const char* name;
    };

    inline constexpr ParticleEmitterShapeMeta particleEmitterShapeMetas[]
    {
#define X(Enum, Display) {ParticleEmitterShape::Enum, Display},
        TMT_PARTICLE_EMITTER_SHAPE_LIST(X)
#undef X
    };
#undef TMT_PARTICLE_EMITTER_SHAPE_LIST

    NLOHMANN_JSON_SERIALIZE_ENUM(ParticleEmitterShape,
        {
            {ParticleEmitterShape::Cone, "Cone"},
            {ParticleEmitterShape::Sphere, "Sphere"},
            {ParticleEmitterShape::Circle, "Circle"}
        }
    )

    //// Particle simulation space
    // World: 이미터의 위치 변화에 영향 X
    // Local: 이미터의 위치 변화에 영향 O (함께 움직임)
#define TMT_PARTICLE_SIMULATION_SPACE_LIST(X)   \
    X(World, "World")                           \
    X(Local, "Local")

    enum class ParticleSimulationSpace
    {
#define X(Enum, Display) Enum,
        TMT_PARTICLE_SIMULATION_SPACE_LIST(X)
#undef X
    };

    struct ParticleSimulationSpaceMeta
    {
        ParticleSimulationSpace type;
        const char* name;
    };

    inline constexpr ParticleSimulationSpaceMeta particleSimulationSpaceMetas[]
            {
#define X(Enum, Display) {ParticleSimulationSpace::Enum, Display},
        TMT_PARTICLE_SIMULATION_SPACE_LIST(X)
#undef X
            };
#undef TMT_PARTICLE_SIMULATION_SPACE_LIST

    NLOHMANN_JSON_SERIALIZE_ENUM(ParticleSimulationSpace,
        {
            {ParticleSimulationSpace::World, "World"},
            {ParticleSimulationSpace::Local, "Local"}
        }
    )
}

#endif //MANGO_PARTICLETYPE_H