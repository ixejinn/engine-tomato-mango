#include "Engine.h"
#include "EngineConfig.h"

#include "Event/EventDispatcher.h"
#include "Simulation/Tick/TickClock.h"
#include "State/DefaultState.h"
#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/SystemFramework/SystemRegistry.h"
#include "GameNetwork/Rollback/RollbackManager.h"
#include "Serialization/ComponentSerializer.h"
#include "Serialization/ComponentRegistry.h"
#include "Editor.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "Utils/Logger.h"
#include "Profiler/Profiler.h"
#include "Profiler/CPUProfiler.h"
#include "Input/KeyDeviceState.h"

namespace tomato {
    Engine::Engine(const int width, const int height, const char* title, NetMode netMode)
        : window_(width, height, title)
        , input_(window_)
        , netMode_(netMode)
    {
        InitializeInputCallbacks();
        InitializeNetwork();

        editor_.InitImGui(window_.GetHandle(), input_);

        Serialization::ComponentRegistry::GetInstance().Init();
        Serialization::ComponentRegistry::GetInstance().InitInspector();
    }

    Engine::~Engine()
    {
        editor_.ShutdownImGui(input_);
    }

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
        auto& systemRegistry = SystemRegistry::GetInstance();
        systemRegistry.RegisterSystems(systemManager_);
        systemRegistry.RegisterEventCallbacks();

        TickClock tickClock;
#ifdef TOMATO_ENGINE
        RunMode runMode{RunMode::Editor};
#else
        RunMode runMode{RunMode::Game};
#endif

        window_.SetWindowUserPointer(input_, tickClock);
        GarbageEntityCollectionSystem garbageCollectionSystem;

        network_->ThreadStart();

        auto& keyState = KeyDeviceState::GetInstance();
        auto& profiler = Profiler::GetInstance();

        while (!window_.ShouldClose() && isRunning_)
        {
            if (nextState_)
                ChangeState(tickClock);

            CPU_PROFILER_TOTAL_BEGIN()

            // std::cout << "       *========== " << tickClock.GetTick() << " ==========*\n";
            ProcessQueuedPackets(tickClock);

            // *---------- Rollback and resimulate
            auto currT = tickClock.GetTick();
            auto lateT = gameNet_->GetConfirmedTick();
            if (currT > lateT &&
                currT - lateT <= ROLLBACK_WINDOW)
            {
                // std::cout << "       Rollback " << lateT << "~" << currT << "\n";

                SimContext rbSimCtx{ currState_.get(), lateT };
                Rollback(rbSimCtx);
                Resimulate(rbSimCtx, currT);
                // std::cout << "       Rollback finish\n";
            }
            // ----------* Rollback and resimulate

            ProcessInputEvents(tickClock.GetTick());

            // *---------- Simulate and render
            SimContext simCtx{ currState_.get(), tickClock.GetTick() };
            garbageCollectionSystem.Update(simCtx);

            FixedUpdate(tickClock, simCtx, runMode);

            Update(simCtx, runMode);
            UpdateEditor(runMode);

            CPU_PROFILER_TOTAL_END()

            window_.SwapBuffers();
            // ----------* Simulate and render

            intentTranslator_.UpdateIntentState(tickClock.GetTick());
        }

        network_->ThreadStop();

        if (profiler.IsActive())
        {
            profiler.SetActive(false);
            profiler.End();
        }
    }

    void Engine::RequestMatchToServer()
    {
        network_->ConnectToServer();
        network_->RequestMatch();
    }

    void Engine::ProcessInputEvents(const uint32_t tick)
    {
        window_.TMP_CheckEscapeKey();
        Window::PollEvents();

        currState_->SetPlayerInput(
            tick, intentTranslator_.GetCurrInputState(),
            network_->GetMyPlayerID());
    }

    void Engine::FixedUpdate(TickClock& tc, SimContext& simCtx, RunMode mode)
    {
        int cnt = tc.GetSimulateNum();
        while (cnt--)
        {
            simCtx.tick = tc.GetTick();
            //std::cout << "\n       *--------- " << simCtx.tick << " ---------*\n";

            systemManager_.FixedUpdate(simCtx, mode);

            EventDispatcher::GetInstance().Update();
            currState_->Update();   // !!!!!! temporary !!!!!!

            if (network_->GetNetState() == ClientNetworkState::NSS_Playing)
                gameNet_->ProcessOutgoingMessages(simCtx.tick);

            ++simCtx.tick;
            tc.AddTick();

            rollbackManager_->Capture(simCtx);
        }
    }

    void Engine::Update(SimContext& simCtx, RunMode mode)
    {
        CPU_PROFILER_BLOCK_BEGIN(Update);
        systemManager_.Update(simCtx, mode);
        CPU_PROFILER_BLOCK_END(Update);
    }

    void Engine::UpdateEditor(RunMode& mode)
    {
        editor_.BeginFrame();
        editor_.Draw(currState_.get(), mode);
        editor_.EndFrame();
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

        TMT_DEBUG << "Key for current State's factory function: " << std::type_index(typeid(*currState_)).name();

        inputUI_.SetState(currState_.get());

        tc.ResetTick();
        SimContext simCtx{ currState_.get(), tc.GetTick() };
        systemManager_.Update(TickPhase::PostUpdate, simCtx, RunMode::Game | RunMode::Editor);

        if (netMode_ == NetMode::NM_Client)
        {
            gameNet_->SetState(currState_.get());
            rollbackManager_->Capture(simCtx);
        }
    }

    void Engine::ProcessQueuedPackets(TickClock& tc)
    {
        gameNet_->InitializeConfirmedTick(tc.GetTick()); // for rollback

        network_->ProcessQueuedUDPPacket();
        network_->ProcessQueuedTCPPacket();
    }

    void Engine::Rollback(SimContext& simCtx)
    {
        rollbackManager_->Rollback(simCtx);
        systemManager_.Update(TickPhase::PostUpdate, simCtx, RunMode::Rollback);
    }

    void Engine::Resimulate(SimContext& simCtx, const uint32_t currT)
    {
        while (simCtx.tick < currT)
        {
            // std::cout << "        --------- " << rbSimCtx.tick << " ---------\n";
            systemManager_.FixedUpdate(simCtx, RunMode::Rollback);
            currState_->Update();   // !!!!!! temporary !!!!!!

            ++simCtx.tick;
            rollbackManager_->Capture(simCtx);
        }
    }

    void Engine::InitializeInputCallbacks()
    {
        input_.keySignal_.Connect<&IntentTranslator::OnKeyEvent>(intentTranslator_);
        input_.cursorSignal_.Connect<&InputUI::OnHover>(inputUI_);
        input_.mouseSignal_.Connect<&IntentTranslator::OnMouseButtonEvent>(intentTranslator_);
        input_.mouseSignal_.Connect<&InputUI::OnClick>(inputUI_);
    }

    void Engine::InitializeNetwork()
    {
        network_ = std::make_unique<ClientNetwork>();
        gameNet_ = std::make_unique<GamePlayNetSystem>(currState_.get());
        network_->SetGameplaySystem(gameNet_.get());
        gameNet_->SetNetwork(network_.get());

        if (!rollbackManager_)
            rollbackManager_ = std::make_unique<RollbackManager>();
    }
}
