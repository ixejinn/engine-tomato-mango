#ifndef MANGO_KEYSTATEMAP_H
#define MANGO_KEYSTATEMAP_H

#include "Containers/EnumArray.h"
#include "Input/KeyConstants.h"
#include "Input/InputEventsFwd.h"

namespace tomato
{
    /// Generic scalar value for a Key, covering both digital and analog input.
    /// 버튼은 released일 때 0.f, pressed일 때 1.f를, 축 키는 연속값을 저장한다.
    using InputAxis = float;

    /**
     * @brief Latest raw value of Keys.
     *
     * 입력 체인에서 에디터 다음으로 연결되어 게임에 사용된다.
     */
    class KeyStateMap
    {
        KeyStateMap() = default;

    public:
        ~KeyStateMap() = default;

        KeyStateMap(const KeyStateMap&) = delete;
        KeyStateMap& operator=(const KeyStateMap&) = delete;

        static KeyStateMap& GetInstance()
        {
            static KeyStateMap instance;
            return instance;
        }

        bool OnKeyEvent(const KeyEvent& event);
        bool OnMouseButtonEvent(const MouseButtonEvent& event);
        bool OnCursorPosEvent(const CursorPosEvent& event);

        InputAxis GetKeyValue(Key key) const { return latestValues_[key]; }

        bool IsKeyDown(Key key) const;
        bool IsKeyUp(Key key) const;

    private:
        void UpdateInputAxis(Key key, InputAxis value);

        EnumArray<Key, InputAxis> latestValues_;
    };
}

#endif //MANGO_KEYSTATEMAP_H