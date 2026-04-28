#ifndef MANGO_INPUTRECORDER_H
#define MANGO_INPUTRECORDER_H

#include "Containers/EnumArray.h"
#include "Input/KeyConstants.h"
#include "Input/InputConstants.h"
#include "Input/InputRecord.h"
#include "Input/InputEventFwd.h"

namespace tomato {
    struct InputAxis {
        float value;
    };

    class InputRecorder {
    public:
        InputRecorder();

        bool UpdateInputKey(const KeyEvent& event);
        bool UpdateInputMouse(const MouseEvent& event);

        InputAxis GetKeyState(Key key) const { return keyStates_[key]; }

        void BindInputIntent(Key key, InputIntent intent) { keyIntents_[key] = intent; }
        InputIntent GetBoundInputIntent(Key key) { return keyIntents_[key]; }

        void UpdateCurrInputRecord(uint32_t tick);
        const InputRecord& GetCurrInputRecord() const { return curr_; }

        bool IsPress(InputIntent intent) const;

    private:
        void InitKeyIntents();

        EnumArray<Key, InputIntent> keyIntents_;
        EnumArray<Key, InputAxis> keyStates_;

        InputRecord prev_;
        InputRecord curr_;
    };
}

#endif //MANGO_INPUTRECORDER_H