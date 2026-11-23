#include "Engine/Engine.h"
#include "Engine/EngineConfig.h"
#include "Engine/EngineDebug.h"
#include "Event/EventDispatcher.h"
#include "Simulation/Tick/TickClock.h"
#include "Render/GLStateCache.h"
#include "State/State.h"
#include "State/StateEvent.h"
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
#include "Input/InputState.h"
namespace tomato
{
    Engine::Engine(const int width, const int height, const char* title, NetMode netMode)
        : window_(width, height, title)
        , input_(window_)
        , netMode_(netMode)
    {
        ConnectInputChain();

        InitializeSerialization();

        editor_.InitImGui(window_.GetHandle(), input_, this);

        EventDispatcher::GetInstance().Connect<Event::ChangeStateRequest, &Engine::OnChangeStateRequest>(*this);
        EventDispatcher::GetInstance().Connect<Event::NetworkModeRequest, &Engine::OnNetworkModeRequest>(*this);
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
        if (!networkRuntime_.HasClient())
            return;

        auto& network_ = networkRuntime_.Client().Network();
        if (network_.GetNetState() == ClientNetworkState::NSS_Starting)
        {
            ServerTimeMs startTime = network_.GetLocalStartTime();
            if (startTime == 0) return;
            auto now = static_cast<ServerTimeMs>(
                duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());

            if (static_cast<int32_t>(now - startTime) >= 0)
            {
                network_.SetNetState(ClientNetworkState::NSS_Playing);
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

        auto& profiler = Profiler::GetInstance();

        ChangeState(tickClock);
        while (!window_.ShouldClose() && isRunning_)
        {
            if (nextState_)
                ChangeState(tickClock);

            // std::cout << "       *========== " << tickClock.GetTick() << " ==========*\n";
            CPU_PROFILER_TOTAL_BEGIN()

            networkRuntime_.ApplyPending(currState_.get()); // 네트워크 모드 지연 적용
            networkRuntime_.PollIncoming(tickClock); // 패킷 처리

            // *---------- Rollback and resimulate
            auto currT = tickClock.GetTick();
            int64_t lateT;
            if (networkRuntime_.ShouldRollback(currT, lateT))
            {
                // std::cout << "       Rollback " << lateT << "~" << currT << "\n";
                SimContext rbSimCtx{ currState_.get(), lateT };
                Rollback(rbSimCtx);
                Resimulate(rbSimCtx, currT);
                // std::cout << "       Rollback finish\n";
            }
            // ----------* Rollback and resimulate

            ProcessInputEvents();

            // *---------- Simulate and render
            SimContext simCtx{ currState_.get(), tickClock.GetTick() };
            garbageCollectionSystem.Update(simCtx);

            FixedUpdate(tickClock, simCtx, runMode);

            Update(simCtx, runMode);
            UpdateEditor(tickClock.GetTick(), runMode);

            CPU_PROFILER_TOTAL_END()

            window_.SwapBuffers();
            // ----------* Simulate and render
        }

        //networkRuntime_.Stop();

        if (profiler.IsActive())
        {
            profiler.SetActive(false);
            profiler.End();
        }
    }

    void Engine::RequestMatchToServer()
    {
        if (!networkRuntime_.HasClient())
            return;

        if (!networkRuntime_.Client().Network().ConnectToServer())
        {
            std::cout << "Can not Connect Server. Please check if the server is active and try again.\n";
            return;
        }
        networkRuntime_.Client().Network().RequestMatch();
    }

    void Engine::SendTCPPacketToServer(TCPPacketType type)
    {
        if (!networkRuntime_.HasClient())
            return;

        switch (type)
        {
        case TCPPacketType::MATCH_LEAVE:
            networkRuntime_.Client().Network().LeaveMatch();
            break;

        case TCPPacketType::GAME_END:
            networkRuntime_.Client().Network().EndGame();
            break;

        default:
            networkRuntime_.Client().Network().SendTCPPacket(type);
        }
    }

    void Engine::ConnectInputChain()
    {
        auto& inputState = InputState::GetInstance();

        // EnTT::sigh는 뒤에 연결한 리스너부터 호출
        // Editor - InputState - UI - Game
        input_.keySignal_.Connect<&PlayerInput::OnKeyEvent>(playerInput_);
        input_.keySignal_.Connect<&InputState::OnKeyEvent>(inputState);
        input_.keySignal_.Connect<&Editor::OnKeyEvent>();

        input_.cursorSignal_.Connect<&InputUI::OnHover>(inputUI_);
        input_.cursorSignal_.Connect<&InputState::OnCursorPosEvent>(inputState);

        input_.mouseSignal_.Connect<&PlayerInput::OnMouseButtonEvent>(playerInput_);
        input_.mouseSignal_.Connect<&InputUI::OnClick>(inputUI_);
        input_.mouseSignal_.Connect<&InputState::OnMouseButtonEvent>(inputState);
        input_.mouseSignal_.Connect<&Editor::OnMouseButtonEvent>();
    }

    void Engine::ProcessInputEvents()
    {
        window_.TMP_CheckEscapeKey();
        window_.PollEvents();
    }

    void Engine::FixedUpdate(TickClock& tc, SimContext& simCtx, RunMode mode)
    {
        int cnt = tc.GetSimulateNum(TickPolicy::Discard);
        while (cnt--)
        {
            simCtx.tick = tc.GetTick();
            TICK_LOG("\n *--------- " << simCtx.tick << " ---------*\n");

            playerInput_.UpdateSnapshot(simCtx.tick);
            currState_->SetPlayerInput(
                    simCtx.tick, playerInput_.GetSnapshot(),
                networkRuntime_.GetMyPlayerID());

            systemManager_.FixedUpdate(simCtx, mode);

            EventDispatcher::GetInstance().Update();
            currState_->Update();   // !!!!!! temporary !!!!!!

            networkRuntime_.ProcessOutgoingMessages(simCtx.tick);

            ++simCtx.tick;
            tc.AddTick();

            networkRuntime_.CaptureSnapshot(simCtx);
        }
    }

    void Engine::Update(SimContext& simCtx, RunMode mode)
    {
        systemManager_.Update(simCtx, mode);
    }

    void Engine::UpdateEditor(uint64_t tick, RunMode& mode)
    {
        editor_.BeginFrame();
        editor_.Draw(tick, currState_.get(), mode);
        editor_.EndFrame();

        // GL 상태 값 초기화
        auto& renderCtx = currState_->GetRegistry().ctx().get<RenderContext>();
        if (currState_)
            renderCtx.glState.Invalidate();

        renderCtx.runMode = mode;
    }

    void Engine::OnChangeStateRequest(const Event::ChangeStateRequest& event)
    {
        loaded = true;

        // 직렬화함수 호출하면서 Engine& 매개변수로 넘기기
        // 직렬화함수에서 직렬화 + 레지스트리 세팅 후 engine.SetNextState로 등록
        serializer_.LoadState(*this, event.filepath.string().c_str());
    }

    void Engine::OnNetworkModeRequest(const Event::NetworkModeRequest& event)
    {
        networkRuntime_.RequestClient(event.client);
        //networkRuntime_.RequestServer(event.server);
    }

    void Engine::ChangeState(TickClock& tc)
    {
        if (!nextState_)
        {
            if (!currState_)
                nextState_ = std::make_unique<State>(*this);
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
        // TMT_DEBUG << "Key for current State's factory function: " << std::type_index(typeid(*currState_)).name();

        if (!loaded)
            currState_->CreateDefaultEntities();
        loaded = false;
        inputUI_.SetState(currState_.get());

        tc.ResetTick();
        SimContext simCtx{ currState_.get(), tc.GetTick() };
        systemManager_.Update(TickPhase::PostUpdate, simCtx, RunMode::Game | RunMode::Editor);

        networkRuntime_.OnStateChanged(currState_.get(), simCtx);
    }

    void Engine::Rollback(SimContext& simCtx)
    {
        networkRuntime_.RollBack(simCtx);
        systemManager_.Update(TickPhase::PostUpdate, simCtx, RunMode::Rollback);
    }

    void Engine::Resimulate(SimContext& simCtx, const uint32_t currT)
    {
        while (simCtx.tick < currT)
        {
            // std::cout << "        --------- " << rbSimCtx.tick << " ---------\n";
            systemManager_.FixedUpdate(simCtx, RunMode::Rollback);
            EventDispatcher::GetInstance().Update();
            currState_->Update();   // !!!!!! temporary !!!!!!

            ++simCtx.tick;
            networkRuntime_.CaptureSnapshot(simCtx);
        }
    }

    void Engine::InitializeSerialization()
    {
        Serialization::ComponentRegistry::GetInstance().Init();
    }
}
