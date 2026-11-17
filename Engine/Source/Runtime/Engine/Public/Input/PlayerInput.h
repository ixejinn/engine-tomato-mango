#ifndef MANGO_PLAYERINPUT_H
#define MANGO_PLAYERINPUT_H

#include "Containers/EnumArray.h"
#include "Input/KeyConstants.h"
#include "Input/InputAction.h"
#include "Input/ActionSnapshot.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    /**
     * @brief Translates key input into per-tick ActionSnapshots.
     *
     * 키를 InputAction으로 바꾼다.
     */
    class PlayerInput
    {
    public:
        PlayerInput();

        bool OnKeyEvent(const KeyEvent& event);
        bool OnMouseButtonEvent(const MouseButtonEvent& event);

        void BindAction(Key key, InputAction action) { keyBindings_[key] = action; }
        InputAction GetBoundAction(Key key) { return keyBindings_[key]; }

        /// 틱마다 한 번 호출해 ActionSnapshot을 갱신한다.
        void UpdateSnapshot(int64_t tick);
        const ActionSnapshot& GetSnapshot() const { return curr_; }

        bool WasJustPressed(InputAction action) const;
        bool IsDown(InputAction action) const;

    private:
        void BindDefaultActions();

        /// 바인딩된 키 중 KeyStateMap에서 눌려 있는 키의 InputAction을 구한다.
        InputAction ComputeDown() const;

        EnumArray<Key, InputAction> keyBindings_;

        /// 마지막 UpdateSnapshot 이후 들어온 press 이벤트의 InputAction.
        /// 입력 이벤트는 프레임, 스냅샷은 틱마다 만들어지므로 여기에 모아 두었다가 다음 틱의 UpdateSnapshot에서 pressed로 사용한다.
        /// press 이벤트 하나가 한 틱에서만 유효하도록 한다.
        InputAction pendingPressed_{InputAction::None};

        /// UpdateSnapshot에서 갱신하는 현재의 ActionSnapshot
        ActionSnapshot curr_;
    };
}

#endif //MANGO_PLAYERINPUT_H