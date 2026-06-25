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
#include "Editor.h"

namespace tomato {
    class Engine {
    public:
        Engine(int width, int height, const char* title, NetMode netMode = NetMode::NM_Alone);
        ~Engine();

        void SetNextState(std::unique_ptr<State>&& newState);
        void TryStartGame(std::unique_ptr<State>&& newState);

        void Run() {
            switch (netMode_)
            {
            case NetMode::NM_Alone:
                SingleRun();
                break;

            case NetMode::NM_Client:
                MultiRun();
                break;
            }
        }

        InputRecorder& GetInputRecorder() { return inputRecorder_; }

    private:
        Window window_;
        Editor editor_;

        std::unique_ptr<ClientNetwork> network_{ nullptr };
        std::unique_ptr<GamePlayNetSystem> gameNet_{ nullptr };

        void SingleRun();
        void MultiRun();

        NetMode netMode_;

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