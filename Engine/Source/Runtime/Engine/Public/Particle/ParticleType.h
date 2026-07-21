#ifndef MANGO_PARTICLETYPE_H
#define MANGO_PARTICLETYPE_H

#include <chrono>
#include "Serialization/Json.h"
//#include "Containers/EnumArray.h"

namespace tomato
{
    // Particle effect shape
#define TMT_PARTICLE_EFFECT_SHAPE_LIST(X)   \
        X(Sphere, "Sphere")                 \
        X(Circle, "Circle")                 \
        X(Cone, "Cone")

    enum class ParticleEffectShape
    {
#define X(Enum, Display) Enum,
        TMT_PARTICLE_EFFECT_SHAPE_LIST(X)
#undef X
    };

    struct ParticleEffectShapeMeta
    {
        ParticleEffectShape shape;
        const char* name;
    };

    static constexpr ParticleEffectShapeMeta shapeMetas[]
    {
#define X(Enum, Display) {ParticleEffectShape::Enum, Display},
        TMT_PARTICLE_EFFECT_SHAPE_LIST(X)
#undef X
    };
#undef TMT_PARTICLE_EFFECT_SHAPE_LIST

    NLOHMANN_JSON_SERIALIZE_ENUM(
            ParticleEffectShape,
            {
                {ParticleEffectShape::Sphere, "Sphere"},
                {ParticleEffectShape::Circle, "Circle"},
                {ParticleEffectShape::Cone, "Cone"}
            }
    )

    // Particle simulation space
#define TMT_PARTICLE_SIMULATION_SPACE_LIST(X)   \
    X(World, "World")                           \
    X(Local, "Local")

    enum ParticleSimulationSpace
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

    static constexpr ParticleSimulationSpaceMeta particleSimulationSpaceMetas[]
            {
#define X(Enum, Display) {ParticleSimulationSpace::Enum, Display},
                    TMT_PARTICLE_SIMULATION_SPACE_LIST(X)
#undef X
            };
#undef TMT_PARTICLE_SIMULATION_SPACE_LIST

NLOHMANN_JSON_SERIALIZE_ENUM(
        ParticleSimulationSpace,
        {
            {ParticleSimulationSpace::World, "World"},
            {ParticleSimulationSpace::Local, "Local"}
        }
        )
}

#endif //MANGO_PARTICLETYPE_H