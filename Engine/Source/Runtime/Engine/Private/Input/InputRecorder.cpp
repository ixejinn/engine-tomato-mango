#include "Input/InputRecorder.h"
#include "Input/InputEvent.h"
#include "Utils/BitmaskOperators.h"

namespace tomato {
    InputRecorder::InputRecorder() {
        InitKeyIntents();
    }

    bool InputRecorder::UpdateInputKey(const KeyEvent& event)
    {
        keyStates_[event.key].value = event.value;

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

    bool InputRecorder::UpdateInputMouse(const MouseEvent& event)
    {
        keyStates_[event.key].value = event.value;
        keyStates_[Key::MouseX].value = event.xPos;
        keyStates_[Key::MouseY].value = event.yPos;

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

    void InputRecorder::UpdateCurrInputRecord(uint32_t tick) {
        if (curr_.tick != tick) {
            prev_ = curr_;
            curr_.tick = tick;
            curr_.down = InputIntent::None;
        }
    }

    bool InputRecorder::IsPress(InputIntent intent) const {
        return HasFlag(curr_.down, intent) ? true : false;
    }

    void InputRecorder::InitKeyIntents() {
        keyIntents_[Key::W] = InputIntent::Up;
        keyIntents_[Key::S] = InputIntent::Down;
        keyIntents_[Key::A] = InputIntent::Left;
        keyIntents_[Key::D] = InputIntent::Right;
        keyIntents_[Key::SpaceBar] = InputIntent::Jump;
    }
}
