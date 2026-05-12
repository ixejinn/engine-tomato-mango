#include "Engine.h"
#include "Event/EventDispatcher.h"
#include "Tick/TickClock.h"
#include "State/DefaultState.h"
#include "ECS/SystemUpdateContexts.h"
#include "Utils/Logger.h"

namespace tomato {
    Engine::Engine(const int width, const int height, const char* title, const bool isSingle)
        : window_(width, height, title), input_(window_, inputRecorder_, inputUI_), isSingle_(isSingle) {}

    Engine::~Engine() = default;

    void Engine::SetNextState(std::unique_ptr<State>&& newState) {
        nextState_ = std::move(newState);
    }

    void Engine::SingleRun() {
        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        ChangeState(tickClock);

        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{currState_->GetRegistry(), tickClock.GetTick()};
            InputContext inputCtx{currState_->GetPlayerInputTimelines()};
            Simulate(tickClock, simCtx, inputCtx);

            RenderContext renderCtx{window_.GetWidth(), window_.GetHeight()};
            Render(simCtx, renderCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }
    }

    void Engine::MultiRun() {

    }

    void Engine::ProcessInputEvents(uint32_t tick) {
        window_.TMP_CheckEscapeKey();
        Window::PollEvents();

        currState_->SetPlayerInput(tick, inputRecorder_.GetCurrInputRecord());
    }

    void Engine::Simulate(TickClock& tc, SimContext& simCtx, InputContext& inputCtx) {
        int simulateNum = tc.GetSimulateNum();

        while (simulateNum--) {
            simCtx.tick = tc.GetTick();

            systemManager_.Simulate(simCtx, inputCtx);

            // !!!!!! temporary !!!!!!
            currState_->Update();

            tc.AddTick();
        }
    }

    void Engine::Render(SimContext& simCtx, RenderContext& renderCtx) {
        systemManager_.Render(simCtx, renderCtx);
        window_.SwapBuffers();
    }

    void Engine::ChangeState(TickClock& tc) {
        if (!nextState_) {
            if (!currState_)
                nextState_ = std::make_unique<DefaultState>(*this);
            else {
                TMT_WARN << "Incorrect State configuration.";
                isRunning_ = false;
                return;
            }
        }

        if (currState_)
            currState_->Exit();

        currState_ = std::move(nextState_);
        currState_->Init();

        tc.ResetTick();
    }
}
