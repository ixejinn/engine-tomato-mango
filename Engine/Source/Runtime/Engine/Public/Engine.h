#ifndef MANGO_ENGINE_H
#define MANGO_ENGINE_H

#include <memory>
#include "Services/Window.h"
#include "Services/Input.h"
#include "Input/InputRecorder.h"
#include "Input/InputUI.h"
#include "ECS/SystemManager.h"
#include "State/StateFwd.h"
#include "Network/ClientNetwork.h"
#include "GameNetwork/GamePlayNetSystem.h"
#include "GameNetwork/Rollback/RollbackManager.h"
#include "Editor.h"

namespace tomato
{
    class Engine
    {
    public:
        Engine(int width, int height, const char* title, NetMode netMode = NetMode::NM_Alone);
        ~Engine();

        void SetNextState(std::unique_ptr<State>&& newState);
        void TryStartGame(std::unique_ptr<State>&& newState);

        void Run();

        InputRecorder& GetInputRecorder() { return inputRecorder_; }

        template<typename Component>
        void SetRollbackComponent() {
            if (netMode_ == NetMode::NM_Alone)
                return;

            if (!rollbackManager_)
                rollbackManager_ = std::make_unique<RollbackManager>();

            rollbackManager_->Register<Component>();
        }

        void RequestMatchToServer();

    private:
        void SingleRun();
        void MultiRun();

        Window window_;

        void ProcessInputEvents(uint32_t tick);
        Input input_;
        InputRecorder inputRecorder_;
        InputUI inputUI_;

        void ChangeState(TickClock& tc);
        std::unique_ptr<State> currState_{nullptr};
        std::unique_ptr<State> nextState_{nullptr};

        Editor editor_;

        std::unique_ptr<ClientNetwork> network_{ nullptr };
        std::unique_ptr<GamePlayNetSystem> gameNet_{ nullptr };
        std::unique_ptr<RollbackManager> rollbackManager_{ nullptr };

        NetMode netMode_;

        void Simulate(TickClock& tc, SimContext& simCtx);

        void Render(SimContext& simCtx);
        SystemManager systemManager_;

        bool isRunning_{true};
    };
}

#endif //MANGO_ENGINE_H