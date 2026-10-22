#ifndef MANGO_NARROWPHASE_H
#define MANGO_NARROWPHASE_H

#include <optional>
#include <entt/fwd.hpp>
#include "Collision/CollisionFwd.h"
#include "Collision/ContactData.h"
#include "Collision/Collider/ColliderTypes.h"
#include "Collision/Detection/Narrow/ShapeView.h"
#include "Containers/EnumArray.h"

namespace tomato
{
    using OverlapFn = bool (*)(const ShapeView& a, const ShapeView& b); // for trigger
    using SweepFn   = std::optional<ContactData> (*)(const ShapeView& a, const ShapeView& b, const glm::vec3& ray);

    class NarrowPhase
    {
    public:
        NarrowPhase();
        
        void RegisterOverlap(ColliderType a, ColliderType b, OverlapFn fn);
        void RegisterSweep(ColliderType a, ColliderType b, SweepFn fn);

        std::optional<ContactData> EvaluateContactPair(entt::registry& reg, const ContactPair& pair);

    private:
        template<typename Fn>
        struct Entry
        {
            Fn fn{nullptr};         // nullptr: GJK
            bool reversed{false};   // true   : Fn(b, a)
        };

        template<typename Fn>
        using Table = EnumArray<ColliderType, EnumArray<ColliderType, Entry<Fn>>>;

        template<typename Fn>
        static void Register(Table<Fn>& table, ColliderType a, ColliderType b, Fn fn)
        {
            table[a][b] = Entry<Fn>{fn, false};

            if (a != b)
                table[b][a] = Entry<Fn>{fn, true};
        }

        bool Overlap(const ShapeView& a, const ShapeView& b);
        std::optional<ContactData> Sweep(const ShapeView& a, const ShapeView& b, const glm::vec3& ray);

        Table<OverlapFn> overlap_;
        Table<SweepFn> sweep_;
    };
}

#endif //MANGO_NARROWPHASE_H