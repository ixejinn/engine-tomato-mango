#include "Engine.h"
#include "Event/EventDispatcher.h"
#include "Tick/TickClock.h"
#include "State/DefaultState.h"
#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/SystemUpdateContexts.h"
#include "GameNetwork/Rollback/RollbackManager.h"
#include "Serialization/ComponentRegistry.h"

#include "Editor.h"
#include "Utils/Logger.h"

namespace tomato {
    Engine::Engine(const int width, const int height, const char* title, NetMode netMode)
        : window_(width, height, title), input_(window_, inputRecorder_, inputUI_), netMode_(netMode), network_(nullptr), gameNet_(nullptr)
    {
        Serialization::ComponentRegistry::GetInstance().Init();
    }

    Engine::~Engine() = default;

    void Engine::SetNextState(std::unique_ptr<State>&& newState)
    {
        nextState_ = std::move(newState);
    }

    void Engine::SingleRun()
    {
        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        GarbageEntityCollectionSystem garbageCollectionSystem;
        ChangeState(tickClock);

        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);

            // TMT_INFO << " ---------- " << tickClock.GetTick() << " ---------- ";
            SimContext simCtx{currState_->GetRegistry(), tickClock.GetTick()};
            InputContext inputCtx{currState_->GetPlayerInputTimelines()};
            // currState_->GetRegistry().ctx().insert_or_assign<InputContext*>(&inputCtx);
            // currState_->GetRegistry().ctx().insert_or_assign<RenderContext*>(&renderCtx);

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            garbageCollectionSystem.Update(simCtx);

            Simulate(tickClock, simCtx, inputCtx);

            RenderContext renderCtx{window_.GetWidth(), window_.GetHeight()};
            Render(simCtx, renderCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }
    }

    void Engine::MultiRun() {
        network_ = std::make_unique<ClientNetwork>();
        gameNet_ = std::make_unique<GamePlayNetSystem>(currState_.get());
        network_->SetGameplaySystem(gameNet_.get());
        gameNet_->SetNetwork(network_.get());

        if (!rollbackManager_)
            rollbackManager_ = std::make_unique<RollbackManager>();

        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        GarbageEntityCollectionSystem garbageCollectionSystem;
        ChangeState(tickClock);
        editor_.InitImGui(window_.GetHandle());

        network_->ThreadStart();

        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);

            // std::cout << "       ========== " << tickClock.GetTick() << " ==========\n";
            InputContext inputCtx{ currState_->GetPlayerInputTimelines() };

            gameNet_->InitializeConfirmedTick(tickClock.GetTick()); // for rollback
            network_->ProcessQueuedUDPPacket();
            network_->ProcessQueuedTCPPacket();

            // *---------- Rollback and resimulate
            auto currT = tickClock.GetTick();
            auto lateT = gameNet_->GetConfirmedTick();
            if (currT > lateT && currT - lateT <= ROLLBACK_WINDOW) {
                // std::cout << "     Rollback start from " << lateT << "\n;
                SimContext rbSimCtx{currState_->GetRegistry(), lateT};

                // Rollback
                rollbackManager_->Rollback(currState_->GetRegistry(), lateT);
                systemManager_.InitializeTransform(rbSimCtx);

                // Resimulation
                while (rbSimCtx.tick < currT) {
                    // std::cout << "       ---------- " << rbSimCtx.tick << " ----------\n";
                    systemManager_.Simulate(rbSimCtx, inputCtx);
                    currState_->Update();   // !!!!!! temporary !!!!!!

                    ++rbSimCtx.tick;
                    rollbackManager_->Capture(rbSimCtx);
                }
                // std::cout << "     Rollback finish\n";
            }
            // ----------* Rollback and resimulate

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{ currState_->GetRegistry(), tickClock.GetTick() };
            garbageCollectionSystem.Update(simCtx);

            // *---------- Simulate
            int cnt = tickClock.GetSimulateNum();
            while (cnt--) {
                simCtx.tick = tickClock.GetTick();

                systemManager_.Simulate(simCtx, inputCtx);
                currState_->Update();   // !!!!!! temporary !!!!!!

                gameNet_->ProcessOutgoingMessages(simCtx.tick);

                ++simCtx.tick;
                rollbackManager_->Capture(simCtx);

                tickClock.AddTick();
            }
            // ----------* Simulate

            RenderContext renderCtx{ window_.GetWidth(), window_.GetHeight() };
            Render(simCtx, renderCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }
        editor_.ShutdownImGui();

        network_->ThreadStop();
    }

    void Engine::ProcessInputEvents(uint32_t tick) {
        window_.TMP_CheckEscapeKey();
        Window::PollEvents();

        currState_->SetPlayerInput(
            tick, inputRecorder_.GetCurrInputRecord(),
            network_ == nullptr ? 0 : network_->GetMyPlayerID());
    }

    void Engine::Simulate(TickClock& tc, SimContext& simCtx, InputContext& inputCtx) {
        int simulateNum = tc.GetSimulateNum();

        while (simulateNum--) {
            simCtx.tick = tc.GetTick();

            systemManager_.Simulate(simCtx, inputCtx);
            currState_->Update();   // !!!!!! temporary !!!!!!

            tc.AddTick();
        }
    }

    void Engine::RequestMatchToServer()
    {
        network_->ConnectToServer();
        network_->RequestMatch();
    }

    void Engine::Render(SimContext& simCtx, RenderContext& renderCtx)
    {
        editor_.BeginFrame();

        systemManager_.Render(simCtx, renderCtx);

        editor_.Draw(currState_.get());
        editor_.EndFrame();

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
        currState_->GetRegistry().ctx().emplace<CollisionContext>();

        tc.ResetTick();

        SimContext simCtx{currState_->GetRegistry(), tc.GetTick()};
        systemManager_.InitializeTransform(simCtx);

        if (netMode_ == NetMode::NM_Client) // !!!!!!!!!! temporary !!!!!!!!!!!
        {
            gameNet_->SetState(currState_.get());
            rollbackManager_->Capture(simCtx);
        }
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
                SetNextState(std::move(newState));
                std::cout << now << "\n##### Game Start #####\n\n";
            }
        }
    }
}
