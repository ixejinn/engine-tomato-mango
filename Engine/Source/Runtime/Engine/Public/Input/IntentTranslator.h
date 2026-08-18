#ifndef MANGO_INTENTTRANSLATOR_H
#define MANGO_INTENTTRANSLATOR_H

#include "Containers/EnumArray.h"
#include "Input/KeyConstants.h"
#include "Input/InputIntent.h"
#include "Input/IntentState.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    /**
     * @brief Translates raw key input into per-tick InputIntent snapshots for the simulation.
     * InputIntent 상태를 조회할 수 있는 유일한 창구.
     */
    class IntentTranslator
    {
    public:
        IntentTranslator();

        bool OnKeyEvent(const KeyEvent& event);
        bool OnMouseButtonEvent(const MouseButtonEvent& event);

        void BindInputIntent(Key key, InputIntent intent) { keyIntents_[key] = intent; }
        InputIntent GetBoundInputIntent(Key key) { return keyIntents_[key]; }

        void UpdateIntentState(uint32_t tick);
        const IntentState& GetCurrInputState() const { return curr_; }

        bool IsPress(InputIntent intent) const;
        bool IsHeld(InputIntent intent) const;

    private:
        void BindInitialInputIntent();

        EnumArray<Key, InputIntent> keyIntents_;

        IntentState prev_;
        IntentState curr_;
    };
}

#endif //MANGO_INTENTTRANSLATOR_H