#ifndef MANGO_COMPONENTTIMELINE_H
#define MANGO_COMPONENTTIMELINE_H

#include <entt/entt.hpp>
#include <vector>
#include "Containers/Timeline.h"
#include "GameNetwork/Rollback/RollbackConfig.h"
#include "ECS/Components/Rollback.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionFwd.h"
#include "Utils/Logger.h"

namespace tomato {
    class ComponentTimelineBase {
    public:
        virtual ~ComponentTimelineBase() = default;

        virtual void Restore(entt::registry&, uint32_t tick) = 0;
        virtual void Record(entt::registry&, uint32_t tick) = 0;
    };

    template<typename Component>
    class ComponentTimeline : public ComponentTimelineBase{
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
    class ComponentTimeline<CollisionPair> : public ComponentTimelineBase {
    public:
        void Restore(entt::registry& reg, uint32_t tick) override {
            const uint32_t storedTick = data_[tick].tick;
            if (storedTick != tick) {
                TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
                return;
            }

            auto& collisionPairs = reg.ctx().get<CollisionContext>().collisionPairs;
            collisionPairs.clear();

            for (auto& pb : data_[tick].data)
                collisionPairs[pb.first] = false;
        }

        void Record(entt::registry& reg, uint32_t tick) override {
            auto& slice = data_[tick];

            slice.tick = tick;

            auto& collisionPairs = reg.ctx().get<CollisionContext>().collisionPairs;
            slice.data.reserve(collisionPairs.size());
            for (auto it = collisionPairs.begin(); it != collisionPairs.end(); ++it)
                slice.data.emplace_back(*it);
        }

    private:
        struct TimelineSlice {
            uint32_t tick;
            std::vector<std::pair<CollisionPair, bool>> data;
        };

        Timeline<TimelineSlice> data_;
    };

    //
    // template<>
    // inline void ComponentTimeline<TransformComponent>::Record(entt::registry& reg, uint32_t tick) {
    //     auto& slice = data_[tick];
    //
    //     slice.tick = tick;
    //
    //     auto view = reg.view<TransformComponent, RollbackEntityTag>();
    //     slice.data.clear();
    //     slice.data.reserve(view.size_hint());
    //
    //     for (auto [e, component] : view.each()) {
    //         component.AddPosition(glm::vec3{0.f, 0.f, 0.f});
    //         slice.data.emplace_back(e, component);
    //
    //         auto pos = component.GetLocalPosition();
    //         // TMT_INFO << " rec " << (int)e << " transform pos(" << tick << "): " << pos.x << " " << pos.y << " " << pos.z;
    //     }
    //     // TMT_INFO << "size: " << slice.data.size();
    // }

    // template<>
    // inline void ComponentTimeline<VelocityComponent>::Record(entt::registry& reg, uint32_t tick) {
    //     auto& slice = data_[tick];
    //
    //     slice.tick = tick;
    //
    //     auto view = reg.view<VelocityComponent, RollbackEntityTag>();
    //     slice.data.clear();
    //     slice.data.reserve(view.size_hint());
    //
    //     for (auto [e, component] : view.each()) {
    //         slice.data.emplace_back(e, component);
    //
    //         auto pos = component.velocity;
    //         // TMT_INFO << " rec " << (int)e << " velocity(" << tick << "): " << pos.x << " " << pos.y << " " << pos.z;
    //     }
    //     // TMT_INFO << "size: " << slice.data.size();
    // }

    // template<>
    // inline void ComponentTimeline<TransformComponent>::Restore(entt::registry& reg, uint32_t tick) {
    //     const uint32_t storedTick = data_[tick].tick;
    //     if (storedTick != tick) {
    //         TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
    //         return;
    //     }
    //
    //     for (auto& [e, component] : data_[tick].data) {
    //         if (reg.all_of<TransformComponent>(e))
    //             reg.get<TransformComponent>(e) = component;
    //         else
    //             reg.emplace<TransformComponent>(e, component);
    //
    //         auto pos = component.GetLocalPosition();
    //         // TMT_INFO << " res " << (int)e << " transform pos(" << tick << "): " << pos.x << " " << pos.y << " " << pos.z;
    //     }
    // }

    // template<>
    // inline void ComponentTimeline<VelocityComponent>::Restore(entt::registry& reg, uint32_t tick) {
    //     const uint32_t storedTick = data_[tick].tick;
    //     if (storedTick != tick) {
    //         TMT_WARN << "Rollback tick mismatch (requested: " << tick << ", stored: " << storedTick << ")";
    //         return;
    //     }
    //
    //     for (auto& [e, component] : data_[tick].data) {
    //         if (reg.all_of<VelocityComponent>(e))
    //             reg.get<VelocityComponent>(e) = component;
    //         else
    //             reg.emplace<VelocityComponent>(e, component);
    //
    //         auto pos = component.velocity;
    //         // TMT_INFO << " res " << (int)e << " velocity(" << tick << "): " << pos.x << " " << pos.y << " " << pos.z;
    //     }
    // }
}

#endif //MANGO_COMPONENTTIMELINE_H