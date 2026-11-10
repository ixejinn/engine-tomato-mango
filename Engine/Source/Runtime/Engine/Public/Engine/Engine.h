#ifndef MANGO_ENGINE_H
#define MANGO_ENGINE_H

#include <memory>

#include "Services/Window.h"
#include "Services/Input.h"

#include "Input/IntentTranslator.h"
#include "Input/InputUI.h"

#include "ECS/SystemFramework/SystemManager.h"
#include "Collision/Layer/CollisionLayerMatrix.h"

#include "State/StateFwd.h"
#include "Simulation/Tick/TickFwd.h"

#include "Network/NetworkRuntime.h"
#include "Network/ClientNetwork.h"
#include "Network/NetworkEvents.h"
#include "GameNetwork/GamePlayNetSystem.h"
#include "GameNetwork/Rollback/RollbackManager.h"

#include "Utils/PassKey.h"

#include "Editor.h"
#include "Serialization/SerializeManager.h"

namespace tomato
{
    class Engine
    {
    public:
        Engine(int width, int height, const char* title, NetMode netMode = NetMode::NM_Alone);
        ~Engine();

        void Run();

        Window& GetWindow() { return window_;}

        IntentTranslator& GetInputRecorder() { return intentTranslator_; }
        Serialization::SerializeManager& GetSerializer() { return serializer_; }

        void SetNextState(std::unique_ptr<State>&& newState);
        template<typename Component>
        void SetRollbackComponent();

        void RequestMatchToServer();
        void TryStartGame(std::unique_ptr<State>&& newState);

        CollisionLayerMatrix collisionLayerMtx_;

    private:
        Window window_;

        void ProcessInputEvents(uint32_t tick);
        Input input_;
        IntentTranslator intentTranslator_;
        InputUI inputUI_;

        void OnChangeStateRequest(const Event::ChangeStateRequest& event);
        void ChangeState(TickClock& tc);
        std::unique_ptr<State> currState_{nullptr};
        std::unique_ptr<State> nextState_{nullptr};
        bool loaded{false};

        Editor editor_;

        void OnNetworkModeRequest(const Event::NetworkModeRequest& event);
        NetworkRuntime networkRuntime_;

        void Rollback(SimContext& simCtx);
        void Resimulate(SimContext& simCtx, uint32_t currT);

        NetMode netMode_;

        void FixedUpdate(TickClock& tc, SimContext& simCtx, RunMode mode);
        void Update(SimContext& simCtx, RunMode mode);
        void UpdateEditor(uint64_t tick, RunMode& mode);
        SystemManager systemManager_;
        bool isRunning_{true};

        void InitializeInput();
        void InitializeSerialization(); ////

        Serialization::SerializeManager serializer_;

    };

    template<typename Component>
    void Engine::SetRollbackComponent()
    {
        if (netMode_ == NetMode::NM_Alone)
            return;

        /*if (!networkRuntime_.Client().Rollback)
            rollbackManager_ = std::make_unique<RollbackManager>();*/
        if(networkRuntime_.HasClient())
            networkRuntime_.Client().Rollback().Register<Component>();
    }
}

#endif //MANGO_ENGINE_H