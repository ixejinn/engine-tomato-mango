#ifndef MANGO_ENGINE_H
#define MANGO_ENGINE_H

#include <memory>
#include <vector>

#include "Services/Window.h"
#include "Services/Input.h"

#include "Input/PlayerInput.h"
#include "Input/InputUI.h"

#include "Input/LocalPlayer.h"
#include "GameObject/Character/CharacterInputActions.h"

#include "ECS/SystemFramework/SystemManager.h"
#include "Collision/Layer/CollisionLayerMatrix.h"

#include "State/StateFwd.h"
#include "Simulation/Tick/TickFwd.h"

#include "Network/ClientNetwork.h"
#include "GameNetwork/GamePlayNetSystem.h"
#include "GameNetwork/Rollback/RollbackManager.h"

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

        /// @return i번째 LocalPlayer (0번은 항상 있음)
        LocalPlayer& GetLocalPlayer(std::size_t i) { return localPlayers_.at(i); }
        /// @return NM_Client 이거나 범위 밖이거나 이미 있는 경우 nullptr
        LocalPlayer* AddLocalPlayer(int playerIdx);

        void SetCharacterInputActions(const CharacterInputActions& actions) { characterInputActions_ = actions; }
        const CharacterInputActions& GetCharacterInputActions() const { return characterInputActions_; }

        Serialization::SerializeManager& GetSerializer() { return serializer_; }

        void SetNextState(std::unique_ptr<State>&& newState);
        template<typename Component>
        void SetRollbackComponent();

        void RequestMatchToServer();
        void TryStartGame(std::unique_ptr<State>&& newState);

        CollisionLayerMatrix collisionLayerMtx_;

    private:
        Window window_;

        void ConnectInputChain();
        void ProcessInputEvents();
        Input input_;
        InputUI inputUI_;

        bool OnLocalPlayerKeyEvent(const KeyEvent& event);
        void UpdateLocalPlayerInputs(int64_t tick);
        std::vector<LocalPlayer> localPlayers_;
        CharacterInputActions characterInputActions_;   // for built-in CharacterMovementSystem

        Editor editor_;
        
        void OnChangeStateRequest(const Event::ChangeStateRequest& event);
        void ChangeState(TickClock& tc);
        std::unique_ptr<State> currState_{nullptr};
        std::unique_ptr<State> nextState_{nullptr};
        bool loaded{false};

        void ProcessQueuedPackets(TickClock& tc);
        std::unique_ptr<ClientNetwork> network_{nullptr};
        std::unique_ptr<GamePlayNetSystem> gameNet_{nullptr};

        void Rollback(SimContext& simCtx);
        void Resimulate(SimContext& simCtx, uint32_t currT);
        std::unique_ptr<RollbackManager> rollbackManager_{nullptr};

        NetMode netMode_;

        void FixedUpdate(TickClock& tc, TickPolicy policy, SimContext& simCtx, RunMode mode);
        void Update(SimContext& simCtx, RunMode mode);
        void UpdateEditor(TickClock& tc, RunMode& mode);
        SystemManager systemManager_;
        bool isRunning_{true};

        void InitializeSerialization(); ////
        void InitializeNetwork();

        Serialization::SerializeManager serializer_;

    };

    template<typename Component>
    void Engine::SetRollbackComponent()
    {
        if (netMode_ == NetMode::NM_Alone)
            return;

        if (!rollbackManager_)
            rollbackManager_ = std::make_unique<RollbackManager>();

        rollbackManager_->Register<Component>();
    }
}

#endif //MANGO_ENGINE_H