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

    void Engine::Run()
    {
        switch (netMode_)
        {
        case NetMode::NM_Alone:
            // SingleRun();
            // break;

        case NetMode::NM_Client:
            MultiRun();
            break;

        case NetMode::NM_DedicatedServer:
            TMT_ERR << "Dedicated server not supported.";
            break;
        }
    }

    void Engine::RequestMatchToServer()
    {
        network_->ConnectToServer();
        network_->RequestMatch();
    }

    void Engine::SingleRun()
    {
        // TickClock tickClock;
        // window_.SetWindowUserPointer(&window_, &input_, &tickClock);
        //
        // GarbageEntityCollectionSystem garbageCollectionSystem;
        // ChangeState(tickClock);
        //
        // while (!window_.ShouldClose() && isRunning_) {
        //     if (nextState_)
        //         ChangeState(tickClock);
        //
        //     // TMT_INFO << " ---------- " << tickClock.GetTick() << " ---------- ";
        //     SimContext simCtx{currState_->GetRegistry(), tickClock.GetTick()};
        //     InputContext inputCtx{currState_->GetPlayerInputTimelines()};
        //     // currState_->GetRegistry().ctx().insert_or_assign<InputContext*>(&inputCtx);
        //     // currState_->GetRegistry().ctx().insert_or_assign<RenderContext*>(&renderCtx);
        //
        //     ProcessInputEvents(tickClock.GetTick());
        //     EventDispatcher::GetInstance().Update();
        //
        //     garbageCollectionSystem.Update(simCtx);
        //
        //     Simulate(tickClock, simCtx, inputCtx);
        //
        //     RenderContext renderCtx{window_.GetWidth(), window_.GetHeight()};
        //     Render(simCtx, renderCtx);
        //
        //     inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        // }
    }

    void Engine::MultiRun()
    {
        network_ = std::make_unique<ClientNetwork>();
        gameNet_ = std::make_unique<GamePlayNetSystem>(currState_.get());
        network_->SetGameplaySystem(gameNet_.get());
        gameNet_->SetNetwork(network_.get());

        if (!rollbackManager_)
            rollbackManager_ = std::make_unique<RollbackManager>();

        TickClock tickClock;
        window_.SetWindowUserPointer(input_, tickClock);

        GarbageEntityCollectionSystem garbageCollectionSystem;

        ChangeState(tickClock);
        editor_.InitImGui(window_.GetHandle());

        network_->ThreadStart();

        while (!window_.ShouldClose() && isRunning_)
            {
            if (nextState_)
                ChangeState(tickClock);

            // std::cout << "       ========== " << tickClock.GetTick() << " ==========\n";
            gameNet_->InitializeConfirmedTick(tickClock.GetTick()); // for rollback

            network_->ProcessQueuedUDPPacket();
            network_->ProcessQueuedTCPPacket();

            // *---------- Rollback and resimulate
            auto currT = tickClock.GetTick();
            auto lateT = gameNet_->GetConfirmedTick();
            if (currT > lateT && currT - lateT <= ROLLBACK_WINDOW)
            {
                // std::cout << "     Rollback " << lateT << "~" << currT << "\n";

                // Rollback
                SimContext rbSimCtx{currState_.get(), lateT};

                rollbackManager_->Rollback(rbSimCtx);
                systemManager_.InitializeTransform(rbSimCtx);

                // Resimulation
                while (rbSimCtx.tick < currT)
                {
                    // std::cout << "       ---------- " << rbSimCtx.tick << " ----------\n";
                    systemManager_.Simulate(rbSimCtx);
                    currState_->Update();   // !!!!!! temporary !!!!!!

                    ++rbSimCtx.tick;
                    rollbackManager_->Capture(rbSimCtx);
                }
                // std::cout << "     Rollback finish\n";
            }
            // ----------* Rollback and resimulate

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{currState_.get(), tickClock.GetTick()};
            garbageCollectionSystem.Update(simCtx);

            // *---------- Simulate
            int cnt = tickClock.GetSimulateNum();
            while (cnt--)
            {
                simCtx.tick = tickClock.GetTick();

                systemManager_.Simulate(simCtx);
                currState_->Update();   // !!!!!! temporary !!!!!!

                if (network_->GetNetState() == ClientNetworkState::NSS_Playing)
                    gameNet_->ProcessOutgoingMessages(simCtx.tick);

                ++simCtx.tick;
                tickClock.AddTick();

                rollbackManager_->Capture(simCtx);
            }
            // ----------* Simulate
            Render(simCtx);

            inputRecorder_.UpdateCurrInputRecord(tickClock.GetTick());
        }
        editor_.ShutdownImGui();

        network_->ThreadStop();
    }

    void Engine::ProcessInputEvents(const uint32_t tick)
    {
        window_.TMP_CheckEscapeKey();
        Window::PollEvents();

        currState_->SetPlayerInput(
            tick, inputRecorder_.GetCurrInputRecord(),
            network_->GetMyPlayerID());
    }

    void Engine::Simulate(TickClock& tc, SimContext& simCtx) {
        int simulateNum = tc.GetSimulateNum();

        while (simulateNum--) {
            simCtx.tick = tc.GetTick();

            systemManager_.Simulate(simCtx);
            currState_->Update();   // !!!!!! temporary !!!!!!

            tc.AddTick();
        }
    }

    void Engine::Render(SimContext& simCtx)
    {
        editor_.BeginFrame();

        systemManager_.Render(simCtx);

        editor_.Draw(currState_.get());
        editor_.EndFrame();

        window_.SwapBuffers();
    }

    void Engine::ChangeState(TickClock& tc)
    {
        if (!nextState_)
        {
            if (!currState_)
                nextState_ = std::make_unique<DefaultState>(*this);
            else
            {
                TMT_WARN << "Next state is NULL.";
                isRunning_ = false;
                return;
            }
        }

        if (currState_)
            currState_->Exit();

        currState_ = std::move(nextState_);
        currState_->Init();

        inputUI_.SetState(currState_.get());

        tc.ResetTick();
        SimContext simCtx{currState_.get(), tc.GetTick()};
        systemManager_.InitializeTransform(simCtx);

        if (netMode_ == NetMode::NM_Client)
        {
            gameNet_->SetState(currState_.get());
            rollbackManager_->Capture(simCtx);
        }
    }
}
