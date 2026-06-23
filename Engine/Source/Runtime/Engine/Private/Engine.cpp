#include "Engine.h"
#include "Event/EventDispatcher.h"
#include "Tick/TickClock.h"
#include "State/DefaultState.h"
#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/SystemUpdateContexts.h"
#include "GameNetwork/Rollback/RollbackManager.h"
#include "Utils/Logger.h"

namespace tomato {
    Engine::Engine(const int width, const int height, const char* title, const bool isSingle)
        : window_(width, height, title), input_(window_, inputRecorder_, inputUI_), isSingle_(isSingle), network_(nullptr), gameNet_(nullptr)
    {

    }

    Engine::~Engine() = default;

    void Engine::SetNextState(std::unique_ptr<State>&& newState) {
        nextState_ = std::move(newState);
    }

    void Engine::SingleRun() {
        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        GarbageEntityCollectionSystem garbageCollectionSystem;

        ChangeState(tickClock);

        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);

            // TMT_INFO << " ---------- " << tickClock.GetTick() << " ---------- ";

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{currState_->GetRegistry(), tickClock.GetTick()};
            InputContext inputCtx{currState_->GetPlayerInputTimelines()};

            garbageCollectionSystem.Update(simCtx);

            Simulate(tickClock, simCtx, inputCtx);

            RenderContext renderCtx{window_.GetWidth(), window_.GetHeight()};
            Render(simCtx, renderCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }
    }

    void Engine::MultiRun() {
        network_ = std::make_unique<ClientNetwork>(NetMode::NM_Client);
        gameNet_ = std::make_unique<GamePlayNetSystem>(currState_.get());
        network_->SetGameplaySystem(gameNet_.get());
        gameNet_->SetNetwork(network_.get());

        if (!rollbackManager_)
            rollbackManager_ = std::make_unique<RollbackManager>();

        network_->ThreadStart();

        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        ChangeState(tickClock);

        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);

            auto currTick = tickClock.GetTick();
            gameNet_->ResetLatestTick(currTick);
            network_->ProcessQueuedUDPPacket();

            InputContext inputCtx{ currState_->GetPlayerInputTimelines() };
            auto rollbackTick = gameNet_->GetLatestTick();
            if (rollbackTick < currTick) {
                rollbackManager_->Rollback(currState_->GetRegistry(), rollbackTick);

                SimContext rollbackCtx{currState_->GetRegistry(), rollbackTick};
                while (rollbackCtx.tick < currTick)
                    Simulate(currTick - rollbackTick, rollbackCtx, inputCtx);
            }

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{ currState_->GetRegistry(), tickClock.GetTick() };
//            InputContext inputCtx{ currState_->GetPlayerInputTimelines() };

            // TODO: Add Garbage entity collection system update

            Simulate(tickClock, simCtx, inputCtx);

            RenderContext renderCtx{ window_.GetWidth(), window_.GetHeight() };
            Render(simCtx, renderCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }

        network_->ThreadStop();
    }

    void Engine::ProcessInputEvents(uint32_t tick) {
        window_.TMP_CheckEscapeKey();
        Window::PollEvents();

        currState_->SetPlayerInput(tick, inputRecorder_.GetCurrInputRecord(), network_ == nullptr ? 0 : network_->GetMyPlayerID());
    }

    void Engine::Simulate(TickClock& tc, SimContext& simCtx, InputContext& inputCtx) {
        int simulateNum = tc.GetSimulateNum();

        while (simulateNum--) {
            simCtx.tick = tc.GetTick();

            systemManager_.Simulate(simCtx, inputCtx);

            // !!!!!! temporary !!!!!!
            currState_->Update();

            if (!isSingle_) {
                // !!!!!!!! temporary !!!!!!!!!!
                gameNet_->ProcessOutgoingMessages(simCtx.tick);

                ++simCtx.tick;
                rollbackManager_->Capture(simCtx);
            }

            tc.AddTick();
        }
    }

    void Engine::Simulate(int cnt, SimContext& simCtx, InputContext& inputCtx) {
        while (cnt--) {
            systemManager_.Simulate(simCtx, inputCtx);
            currState_->Update();

            ++simCtx.tick;
            rollbackManager_->Capture(simCtx);
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

        inputUI_.SetState(currState_.get());
        if (!isSingle_) // !!!!!!!!!! temporary !!!!!!!!!!!
            gameNet_->SetState(currState_.get());
        tc.ResetTick();

        SimContext simCtx{currState_->GetRegistry(), tc.GetTick()};
        systemManager_.InitializeTransform(simCtx);
    }

    void Engine::TryStartGame(std::unique_ptr<State>&& newState)
    {
        if (network_->GetNetState() == ClientNetworkState::NSS_Starting)
        {
            ServerTimeMs startTime = network_->GetLocalStartTime();
            if (startTime == 0) return;
            auto now = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());

            if (static_cast<int32_t>(now - startTime) >= 0)
            {
                network_->SetNetState(ClientNetworkState::NSS_Playing);
                //SetNextState(newState);
                std::cout << now << "\n##### Game Start #####\n\n";
            }
        }
    }
}
