#include "Engine.h"
#include "Event/EventDispatcher.h"
#include "Tick/TickClock.h"
#include "State/DefaultState.h"
#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/SystemUpdateContexts.h"
#include "ECS/Components/Hierarchy.h"
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

        TickClock tickClock;
        window_.SetWindowUserPointer(&window_, &input_, &tickClock);

        ChangeState(tickClock);

        network_->ThreadStart();
        while (!window_.ShouldClose() && isRunning_) {
            if (nextState_)
                ChangeState(tickClock);
            InputContext inputCtx{ currState_->GetPlayerInputTimelines() };

            TMT_INFO << "       ========== " << tickClock.GetTick() << " ========== ";

            gameNet_->ResetLatestTick(tickClock.GetTick());
            network_->ProcessQueuedUDPPacket();

            // TODO: Rollback
            auto currT = tickClock.GetTick();
            auto lateT = gameNet_->GetLatestTick();
            if (currT != lateT && currT - lateT <= ROLLBACK_WINDOW) {
                rollbackManager_->Rollback(currState_->GetRegistry(), lateT);
                TMT_INFO << "      Rollback to " << lateT;

                SimContext rbSimCtx{currState_->GetRegistry(), lateT};
                while (rbSimCtx.tick < currT) {
                    std::cout << "           ----- " << rbSimCtx.tick << " ----- \n";
                    systemManager_.Simulate(rbSimCtx, inputCtx);
                    currState_->Update();
                    ++rbSimCtx.tick;

                    rollbackManager_->Capture(rbSimCtx);
                }
                TMT_INFO << "      Rollback finish";
            }
            // Rollback

            ProcessInputEvents(tickClock.GetTick());
            EventDispatcher::GetInstance().Update();

            SimContext simCtx{ currState_->GetRegistry(), tickClock.GetTick() };
            // InputContext inputCtx{ currState_->GetPlayerInputTimelines() };

            // TODO: Add Garbage entity collection system update

            Simulate(tickClock, simCtx, inputCtx);

            // auto view = currState_->GetRegistry().view<TransformComponent, RootEntityTag>();
            // for (auto [e, trs] : view.each()) {
            //     auto pos = trs.GetWorldPosition();
            //     std::cout << (int)e << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
            // }

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
        // if (!isSingle_) // !!!!!!!!!! temporary !!!!!!!!!!!
        //     gameNet_->SetState(currState_.get());

        tc.ResetTick();

        currState_->GetRegistry().ctx().emplace<CollisionContext>();
        SimContext simCtx{currState_->GetRegistry(), tc.GetTick()};
        systemManager_.InitializeTransform(simCtx);

        if (!isSingle_) // !!!!!!!!!! temporary !!!!!!!!!!!
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
                //SetNextState(newState);
                std::cout << now << "\n##### Game Start #####\n\n";
            }
        }
    }
}
