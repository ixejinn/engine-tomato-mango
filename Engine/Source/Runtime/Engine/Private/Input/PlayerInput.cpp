#include <ranges>
#include "Input/PlayerInput.h"
#include "Input/InputEvents.h"
#include "Input/KeyStateMap.h"
#include "Input/InputMappingContext.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    bool PlayerInput::OnKeyEvent(const KeyEvent& event)
    {
        if (event.event == InputEvent::Pressed)
            pendingPressed_ |= keyBindings_[event.key];
        return true;
    }

    void PlayerInput::SetMappings(const InputMappingContext& imc)
    {
        ClearMappings();
        for (const auto& [action, key] : imc.mappings)
            keyBindings_[key] |= action;
    }

    void PlayerInput::ClearMappings()
    {
        for (auto& action : keyBindings_)
            action = InputAction::None;

        pendingPressed_ = InputAction::None;
    }

    void PlayerInput::UpdateSnapshot(int64_t tick)
    {
        curr_.tick = tick;
        curr_.down = ComputeDown();
        curr_.pressed = pendingPressed_;

        pendingPressed_ = InputAction::None;
    }

    bool PlayerInput::WasJustPressed(const InputAction action) const
    {
        return HasFlag(curr_.pressed, action);
    }

    bool PlayerInput::IsDown(const InputAction action) const
    {
        return HasFlag(curr_.down, action);
    }

    InputAction PlayerInput::ComputeDown() const
    {
        const auto& keyStateMap = KeyStateMap::GetInstance();

        InputAction down{InputAction::None};

        auto keyRange =
                std::views::iota(0, static_cast<int>(Key::AXIS_BEGIN)) |
                std::views::transform([](int i) { return static_cast<Key>(i); });
        for (Key key : keyRange)
        {
            if (keyBindings_[key] != InputAction::None && keyStateMap.IsKeyDown(key))
                down |= keyBindings_[key];
        }

        return down;
    }
}
