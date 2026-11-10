#include <ranges>
#include "Input/IntentTranslator.h"
#include "Input/InputEvents.h"
#include "Input/KeyDeviceState.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    IntentTranslator::IntentTranslator()
    {
        BindInitialInputIntent();
    }

    bool IntentTranslator::OnKeyEvent(const KeyEvent& event)
    {
        if (event.action == KeyAction::Press)
            pendingDown_ |= keyIntents_[event.key];
        return true;
    }

    bool IntentTranslator::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (event.action == KeyAction::Press)
            pendingDown_ |= keyIntents_[event.key];
        return true;
    }

    void IntentTranslator::UpdateInputState(int64_t tick)
    {
        curr_.tick = tick;
        curr_.held = ComputeHeld();
        curr_.down = pendingDown_;

        pendingDown_ = InputIntent::None;
    }

    bool IntentTranslator::IsPress(InputIntent intent) const
    {
        return HasFlag(curr_.down, intent);
    }

    bool IntentTranslator::IsHeld(InputIntent intent) const
    {
        return HasFlag(curr_.held, intent);
    }

    void IntentTranslator::BindInitialInputIntent() {
        keyIntents_[Key::W] = InputIntent::Up;
        keyIntents_[Key::S] = InputIntent::Down;
        keyIntents_[Key::A] = InputIntent::Left;
        keyIntents_[Key::D] = InputIntent::Right;
        keyIntents_[Key::SpaceBar] = InputIntent::Jump;

        keyIntents_[Key::Left]  = InputIntent::TurnLeft;
        keyIntents_[Key::Right] = InputIntent::TurnRight;
        keyIntents_[Key::Up]    = InputIntent::TurnUp;
        keyIntents_[Key::Down]  = InputIntent::TurnDown;

        keyIntents_[Key::KP_8] = InputIntent::Up2;
        keyIntents_[Key::KP_5] = InputIntent::Down2;
        keyIntents_[Key::KP_4] = InputIntent::Left2;
        keyIntents_[Key::KP_6] = InputIntent::Right2;
        keyIntents_[Key::KP_0] = InputIntent::Jump2;
    }

    InputIntent IntentTranslator::ComputeHeld() const
    {
        const auto& keyState = KeyDeviceState::GetInstance();

        InputIntent held{InputIntent::None};

        auto keyRange =
                std::views::iota(0, static_cast<int>(Key::AXIS_BEGIN)) |
                std::views::transform([](int i) { return static_cast<Key>(i); });
        for (Key key : keyRange)
        {
            if (keyIntents_[key] != InputIntent::None && keyState.IsKeyPressed(key))
                held |= keyIntents_[key];
        }

        return held;
    }
}
