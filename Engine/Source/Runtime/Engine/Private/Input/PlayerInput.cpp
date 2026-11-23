#include <ranges>
#include "Input/PlayerInput.h"
#include "Input/InputEvents.h"
#include "Input/InputState.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    PlayerInput::PlayerInput()
    {
        BindDefaultActions();
    }

    bool PlayerInput::OnKeyEvent(const KeyEvent& event)
    {
        if (event.type == KeyEventType::Press)
            pendingPressed_ |= keyBindings_[event.key];
        return true;
    }

    bool PlayerInput::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (event.type == KeyEventType::Press)
            pendingPressed_ |= keyBindings_[event.key];
        return true;
    }

    void PlayerInput::UpdateSnapshot(int64_t tick)
    {
        curr_.tick = tick;
        curr_.held = ComputeHeld();
        curr_.pressed = pendingPressed_;

        pendingPressed_ = InputAction::None;
    }

    bool PlayerInput::WasJustPressed(InputAction intent) const
    {
        return HasFlag(curr_.pressed, intent);
    }

    bool PlayerInput::IsDown(InputAction intent) const
    {
        return HasFlag(curr_.held, intent);
    }

    void PlayerInput::BindDefaultActions() {
        keyBindings_[Key::W] = InputAction::Up;
        keyBindings_[Key::S] = InputAction::Down;
        keyBindings_[Key::A] = InputAction::Left;
        keyBindings_[Key::D] = InputAction::Right;
        keyBindings_[Key::SpaceBar] = InputAction::Jump;

        keyBindings_[Key::Left]  = InputAction::TurnLeft;
        keyBindings_[Key::Right] = InputAction::TurnRight;
        keyBindings_[Key::Up]    = InputAction::TurnUp;
        keyBindings_[Key::Down]  = InputAction::TurnDown;

        keyBindings_[Key::KP_8] = InputAction::Up2;
        keyBindings_[Key::KP_5] = InputAction::Down2;
        keyBindings_[Key::KP_4] = InputAction::Left2;
        keyBindings_[Key::KP_6] = InputAction::Right2;
        keyBindings_[Key::KP_0] = InputAction::Jump2;
    }

    InputAction PlayerInput::ComputeHeld() const
    {
        const auto& keyState = InputState::GetInstance();

        InputAction held{InputAction::None};

        auto keyRange =
                std::views::iota(0, static_cast<int>(Key::AXIS_BEGIN)) |
                std::views::transform([](int i) { return static_cast<Key>(i); });
        for (Key key : keyRange)
        {
            if (keyBindings_[key] != InputAction::None && keyState.IsKeyPressed(key))
                held |= keyBindings_[key];
        }

        return held;
    }
}
