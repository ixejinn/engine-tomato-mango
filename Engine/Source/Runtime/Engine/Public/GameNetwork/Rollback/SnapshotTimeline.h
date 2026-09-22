#ifndef MANGO_SNAPSHOTTIMELINE_H
#define MANGO_SNAPSHOTTIMELINE_H

#include <entt/entt.hpp>
#include <vector>
#include <array>
#include "Containers/Timeline.h"
#include "Containers/EntityPool.h"
#include "GameNetwork/Rollback/RollbackConfig.h"
#include "ECS/Components/Rollback.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Utils/Logger.h"

namespace tomato
{
    class SnapshotTimelineBase
    {
    public:
        virtual ~SnapshotTimelineBase() = default;

        virtual void Rollback(entt::registry&, uint32_t tick) = 0;
        virtual void Capture(entt::registry&, uint32_t tick) = 0;
    };

    template<typename Component>
    class SnapshotTimeline : public SnapshotTimelineBase
    {
    public:
        void Rollback(entt::registry& reg, uint32_t tick) override
        {
            const uint32_t storedTick = data_[tick].tick;
            if (storedTick != tick)
            {
                TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
                return;
            }

            for (auto& [e, component] : data_[tick].data)
            {
                if (reg.all_of<Component>(e))
                    reg.get<Component>(e) = component;
                else
                    reg.emplace<Component>(e, component);
            }
        }

        void Capture(entt::registry& reg, uint32_t tick) override
        {
            auto& slice = data_[tick];

            slice.tick = tick;

            auto view = reg.view<Component, RollbackEntityTag>();
            slice.data.clear();
            slice.data.reserve(view.size_hint());

            for (auto [e, component] : view.each())
                slice.data.emplace_back(e, component);
        }

    private:
        struct TimelineSlice
        {
            uint32_t tick;
            std::vector<std::pair<entt::entity, Component>> data;
        };

        Timeline<TimelineSlice> data_;
    };

    template<>
    inline void SnapshotTimeline<TransformComponent>::Capture(entt::registry& reg, uint32_t tick)
    {
        auto& slice = data_[tick];

        slice.tick = tick;

        auto view = reg.view<TransformComponent, RollbackEntityTag>();
        slice.data.clear();
        slice.data.reserve(view.size_hint());

        for (const auto& [e, component] : view.each())
        {
            // component.OnLocalDirtyBit();
            slice.data.emplace_back(e, component);
        }
    }

    template<>
    class SnapshotTimeline<CollisionContext> : public SnapshotTimelineBase
    {
    public:
        void Rollback(entt::registry& reg, uint32_t tick) override
        {
            const uint32_t storedTick = data_[tick].tick;
            if (storedTick != tick)
            {
                TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
                return;
            }

            auto& contactPairs = reg.ctx().get<CollisionContext>().cacheMap;
            // TMT_INFO << "     Restore collision pair " << collisionPairs.size() << " -> " << data_[tick].data.size();
            contactPairs.clear();

            for (auto& pb : data_[tick].data)
                contactPairs[pb.first] = pb.second;
        }

        void Capture(entt::registry& reg, uint32_t tick) override
        {
            auto& slice = data_[tick];

            slice.tick = tick;
            slice.data.clear();

            auto& contactPairs = reg.ctx().get<CollisionContext>().cacheMap;
            slice.data.reserve(contactPairs.size());
            // TMT_INFO << "     Back up collision pair size: " << collisionPairs.size();
            for (auto it = contactPairs.begin(); it != contactPairs.end(); ++it)
                slice.data.emplace_back(*it);
        }

    private:
        struct TimelineSlice
        {
            uint32_t tick;
            std::vector<std::pair<ContactPair, ContactCache>> data;
        };

        Timeline<TimelineSlice> data_;
    };

    template<EntityPoolTraits Traits>
    class SnapshotTimeline<EntityPool<Traits>> : public SnapshotTimelineBase
    {
    public:
        void Rollback(entt::registry& reg, uint32_t tick) override
        {
            auto* pool = reg.ctx().find<EntityPool<Traits>>();
            if (!pool)
            {
                TMT_ERR << "This entity pool is not found in the registry context.";
                return;
            }

            auto& slice = data_[tick];
            pool->entries_ = slice.entries;
            pool->freeIndices_ = slice.freeIndices;
            pool->freeEntityCount_ = slice.freeEntityCount;
        }

        void Capture(entt::registry& reg, uint32_t tick) override
        {
            auto* pool = reg.ctx().find<EntityPool<Traits>>();
            if (!pool)
            {
                TMT_ERR << "This entity pool is not found in the registry context.";
                return;
            }

            auto& slice = data_[tick];

            slice.tick = tick;
            slice.entries = pool->entries_;
            slice.freeIndices_ = pool->freeIndices_;
            slice.freeEntityCount_ = pool->freeEntityCount_;
        }

    private:
        struct TimelineSlice
        {
            uint32_t tick;
            std::array<_entityPoolDetail::Entry, Traits::N> entries;
            std::array<uint32_t, Traits::N> freeIndices;
            uint32_t freeEntityCount;
        };

        Timeline<TimelineSlice> data_;
    };
}

#endif //MANGO_SNAPSHOTTIMELINE_H