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
     * @brief Translates raw key input into per-tick InputAction snapshots for the simulation.
     * InputAction 상태를 조회할 수 있는 유일한 창구.
     */
    class PlayerInput
    {
    public:
        PlayerInput();

        bool OnKeyEvent(const KeyEvent& event);
        bool OnMouseButtonEvent(const MouseButtonEvent& event);

        void BindAction(Key key, InputAction intent) { keyBindings_[key] = intent; }
        InputAction GetBoundAction(Key key) { return keyBindings_[key]; }

        void UpdateSnapshot(int64_t tick);
        const ActionSnapshot& GetSnapshot() const { return curr_; }

        bool WasJustPressed(InputAction intent) const;
        bool IsDown(InputAction intent) const;

    private:
        void BindDefaultActions();

        InputAction ComputeHeld() const;

        EnumArray<Key, InputAction> keyBindings_;

        /// Sample 호출 마다 press 입력 이벤트를 모아 Sample에서 curr_.down를 덮어씀
        InputAction pendingPressed_{InputAction::None};

        /// 틱마다 호출되는 Sample에서 갱신
        /// held: 틱마다 KeyDeviceState에서 현재 눌려있는 키를 조회
        /// down: pendingDown_으로 덮어씀
        ActionSnapshot curr_;
    };
}

#endif //MANGO_PLAYERINPUT_H