#include <random>
#include "TimerTestSystem.h"
#include "TimerTestComponent.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Text.h"
#include "ECS/SystemUpdateContexts.h"
#include "State/State.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Logic, TimerTestSystem)

using namespace tomato;

void TimerTestSystem::Update(SimContext& simCtx)
{
    auto& reg = simCtx.state->GetRegistry();
    auto view = reg.view<TimerTestComponent, RenderComponent, HierarchyComponent>();
    for (auto [e, ttc, render, hierarchy] : view.each())
    {
        if (auto* text = reg.try_get<TextComponent>(hierarchy.children.front()))
        {
            text->text = ttc.timer.GetString(simCtx.tick);
            text->dirty = true;
        }
        else
            TMT_INFO << "other children";

        if (ttc.timer.IsTimeUp(simCtx.tick))
        {
            TMT_INFO << "time up";
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(0, 255);

            render.color = glm::vec4(dis(gen)/255.f, dis(gen)/255.f, dis(gen)/255.f, 1.f);
        }
    }
}