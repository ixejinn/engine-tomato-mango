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

        void UpdateInputState(int64_t tick);

        const IntentState& GetCurrInputState() const { return curr_; }

        bool IsPress(InputIntent intent) const;
        bool IsHeld(InputIntent intent) const;

    private:
        void BindInitialInputIntent();

        InputIntent ComputeHeld() const;

        EnumArray<Key, InputIntent> keyIntents_;

        /// Sample 호출 마다 press 입력 이벤트를 모아 Sample에서 curr_.down를 덮어씀
        InputIntent pendingDown_{InputIntent::None};

        /// 틱마다 호출되는 Sample에서 갱신
        /// held: 틱마다 KeyDeviceState에서 현재 눌려있는 키를 조회
        /// down: pendingDown_으로 덮어씀
        IntentState curr_;
    };
}

#endif //MANGO_INTENTTRANSLATOR_H