#ifndef MANGO_KEYDEVICESTATE_H
#define MANGO_KEYDEVICESTATE_H

#include "Containers/EnumArray.h"
#include "Input/KeyConstants.h"

namespace tomato
{
    /// Generic scalar value for a Key, covering both digital and analog input.
    /// 버튼의 경우, released 0.f/pressed 1.f를, 축 키는 연속값을 저장한다.
    using InputAxis = float;

    /**
     * @brief Raw per-key input cache.
     *
     * 모든 Key에 대해 최신 InputAxis 값을 보관하는 싱글톤 클래스.
     * 게임 모드(게임플레이/에디터)와 무관하게 항상 각 키의 InputAxis는 최신 상태로 유지된다.
     */
    class KeyDeviceState
    {
        KeyDeviceState() = default;

    public:
        ~KeyDeviceState() = default;

        KeyDeviceState(const KeyDeviceState&) = delete;
        KeyDeviceState& operator=(const KeyDeviceState&) = delete;

        static KeyDeviceState& GetInstance()
        {
            static KeyDeviceState instance;
            return instance;
        }

        InputAxis GetKeyState(Key key) { return latestValues_[key]; }

        bool IsKeyPressed(Key key);
        bool IsKeyReleased(Key key);

    private:
        void UpdateInputAxis(Key key, InputAxis value);

        EnumArray<Key, InputAxis> latestValues_;

        friend class Input;
    };
}

#endif //MANGO_KEYDEVICESTATE_H