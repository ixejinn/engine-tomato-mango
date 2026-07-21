#ifndef MANGO_KEYCONSTANTS_H
#define MANGO_KEYCONSTANTS_H

namespace tomato
{
    /**
     * @brief Input code for buttons and axes.
     *
     * 키보드/마우스 버튼과 마우스 축(MouseX/MouseY)을 하나의 enum으로 관리한다.
     *
     * @details
     * AXIS_BEGIN을 기준으로 버튼과 축 영역을 구분하므로,
     * AXIS_BEGIN 자체는 실제 입력 코드가 아닌 경계로 취급한다.
     */
    enum class Key
    {
        UNKNOWN,

        // BUTTONS ------------------------------------------------
        LeftMouseButton,
        RightMouseButton,

        Escape,
        SpaceBar,

        Left,
        Up,
        Right,
        Down,

        K_0,
        K_1,
        K_2,
        K_3,
        K_4,
        K_5,
        K_6,
        K_7,
        K_8,
        K_9,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,

        // AXES ---------------------------------------------------
        AXIS_BEGIN,

        MouseX,
        MouseY,

        COUNT
    };

    inline bool IsButton(Key key) { return key < Key::AXIS_BEGIN; }

    enum class KeyAction
    {
        Release,
        Press,
        Error
    };
}

#endif //MANGO_KEYCONSTANTS_H