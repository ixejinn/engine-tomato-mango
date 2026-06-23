#ifndef MANGO_COMPONENTTIMELINE_H
#define MANGO_COMPONENTTIMELINE_H

#include <entt/entt.hpp>
#include <vector>
#include "Containers/Timeline.h"
#include "GameNetwork/Rollback/RollbackConfig.h"
#include "ECS/Components/Rollback.h"

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
            for (auto& [e, component] : data_[tick]) {
                if (reg.all_of<Component>(e))
                    reg.get<Component>(e) = component;
                else
                    reg.emplace<Component>(e, component);
            }
        }

        void Record(entt::registry& reg, uint32_t tick) override {
            auto& rollbackSlice = data_[tick];

            auto view = reg.view<Component, RollbackEntityTag>();
            data_[tick].clear();
            data_[tick].resize(view.size_hint());

            for (auto [e, component] : view.each())
                rollbackSlice.emplace_back(e, component);
        }

    private:
        Timeline<std::vector<std::pair<entt::entity, Component>>, ROLLBACK_WINDOW> data_;
    };
}

#endif //MANGO_COMPONENTTIMELINE_H