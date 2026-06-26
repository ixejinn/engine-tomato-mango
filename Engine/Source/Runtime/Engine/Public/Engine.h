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

namespace tomato {
    class Engine {
    public:
        Engine(int width, int height, const char* title, bool isSingle = true);
        ~Engine();

        void SetNextState(std::unique_ptr<State>&& newState);
        void TryStartGame(std::unique_ptr<State>&& newState);

        void Run() {
            if (isSingle_)
                SingleRun();
            else
                MultiRun();
        }

        InputRecorder& GetInputRecorder() { return inputRecorder_; }

        template<typename Component>
        void SetRollbackComponent() {
            if (isSingle_)
                return;

            if (!rollbackManager_)
                rollbackManager_ = std::make_unique<RollbackManager>();

            rollbackManager_->Register<Component>();
        }

    private:
        Window window_;

        std::unique_ptr<ClientNetwork> network_{ nullptr };
        std::unique_ptr<GamePlayNetSystem> gameNet_{ nullptr };
        std::unique_ptr<RollbackManager> rollbackManager_{ nullptr };

        void SingleRun();
        void MultiRun();
        bool isSingle_;

        void ProcessInputEvents(uint32_t tick);
        Input input_;
        InputRecorder inputRecorder_;
        InputUI inputUI_;

        void Simulate(TickClock& tc, SimContext& simCtx, InputContext& inputCtx);

        void Render(SimContext& simCtx, RenderContext& renderCtx);
        SystemManager systemManager_;

        void ChangeState(TickClock& tc);
        std::unique_ptr<State> currState_{nullptr};
        std::unique_ptr<State> nextState_{nullptr};

        bool isRunning_{true};
    };
}

#endif //MANGO_ENGINE_H