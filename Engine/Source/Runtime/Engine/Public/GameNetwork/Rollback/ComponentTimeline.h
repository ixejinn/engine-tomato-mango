#ifndef MANGO_COMPONENTTIMELINE_H
#define MANGO_COMPONENTTIMELINE_H

#include <entt/entt.hpp>
#include <vector>
#include "Containers/Timeline.h"
#include "GameNetwork/Rollback/RollbackConfig.h"
#include "ECS/Components/Rollback.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Utils/Logger.h"

namespace tomato {
    class SnapshotTimelineBase {
    public:
        virtual ~SnapshotTimelineBase() = default;

        virtual void Restore(entt::registry&, uint32_t tick) = 0;
        virtual void Record(entt::registry&, uint32_t tick) = 0;
    };

    template<typename Component>
    class SnapshotTimeline : public SnapshotTimelineBase{
    public:
        void Restore(entt::registry& reg, uint32_t tick) override {
            const uint32_t storedTick = data_[tick].tick;
            if (storedTick != tick) {
                TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
                return;
            }

            for (auto& [e, component] : data_[tick].data) {
                if (reg.all_of<Component>(e))
                    reg.get<Component>(e) = component;
                else
                    reg.emplace<Component>(e, component);
            }
        }

        void Record(entt::registry& reg, uint32_t tick) override {
            auto& slice = data_[tick];

            slice.tick = tick;

            auto view = reg.view<Component, RollbackEntityTag>();
            slice.data.clear();
            slice.data.reserve(view.size_hint());

            for (auto [e, component] : view.each())
                slice.data.emplace_back(e, component);
        }

    private:
        struct TimelineSlice {
            uint32_t tick;
            std::vector<std::pair<entt::entity, Component>> data;
        };

        Timeline<TimelineSlice> data_;
    };

    template<>
    inline void SnapshotTimeline<TransformComponent>::Record(entt::registry& reg, uint32_t tick) {
        auto& slice = data_[tick];

        slice.tick = tick;

        auto view = reg.view<TransformComponent, RollbackEntityTag>();
        slice.data.clear();
        slice.data.reserve(view.size_hint());

        for (auto [e, component] : view.each()) {
            component.AddPosition(glm::vec3{0.f, 0.f, 0.f});
            slice.data.emplace_back(e, component);
        }
    }

    template<>
    class SnapshotTimeline<CollisionContext> : public SnapshotTimelineBase {
    public:
        void Restore(entt::registry& reg, uint32_t tick) override {
            const uint32_t storedTick = data_[tick].tick;
            if (storedTick != tick) {
                TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
                return;
            }

            auto& contactPairs = reg.ctx().get<CollisionContext>().cacheMap;
            // TMT_INFO << "     Restore collision pair " << collisionPairs.size() << " -> " << data_[tick].data.size();
            contactPairs.clear();

            for (auto& pb : data_[tick].data)
                contactPairs[pb.first] = pb.second;
        }

        void Record(entt::registry& reg, uint32_t tick) override {
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
        struct TimelineSlice {
            uint32_t tick;
            std::vector<std::pair<ContactPair, ContactCache>> data;
        };

        Timeline<TimelineSlice> data_;
    };
}

#endif //MANGO_COMPONENTTIMELINE_H