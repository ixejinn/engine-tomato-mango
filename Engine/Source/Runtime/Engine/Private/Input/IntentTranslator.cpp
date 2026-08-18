#include "Input/IntentTranslator.h"
#include "Input/InputEvent.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    IntentTranslator::IntentTranslator()
    {
        BindInitialInputIntent();
    }

    bool IntentTranslator::OnKeyEvent(const KeyEvent& event)
    {
        if (keyIntents_[event.key] != InputIntent::None)
        {
            if (event.action == KeyAction::Release)
                curr_.held &= ~keyIntents_[event.key];
            else             // KeyAction::Press
            {
                if (!HasFlag(prev_.held, keyIntents_[event.key]))
                    curr_.down |= keyIntents_[event.key];
                curr_.held |= keyIntents_[event.key];
            }
        }

        return true;
    }

    bool IntentTranslator::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (keyIntents_[event.key] != InputIntent::None)
        {
            if (event.action == KeyAction::Release)
                curr_.held &= ~keyIntents_[event.key];
            else             // KeyAction::Press
            {
                if (!HasFlag(prev_.held, keyIntents_[event.key]))
                    curr_.down |= keyIntents_[event.key];
                curr_.held |= keyIntents_[event.key];
            }
        }

        return true;
    }

    void IntentTranslator::UpdateIntentState(uint32_t tick)
    {
        if (curr_.tick != tick)
        {
            prev_ = curr_;
            curr_.tick = tick;
            curr_.down = InputIntent::None;
        }
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
}
