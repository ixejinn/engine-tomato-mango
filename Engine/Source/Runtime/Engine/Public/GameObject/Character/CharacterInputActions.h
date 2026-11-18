#ifndef MANGO_CHARACTERINPUTACTIONS_H
#define MANGO_CHARACTERINPUTACTIONS_H

#include "Input/InputAction.h"

namespace tomato
{
    /**
     * @brief InputAction set for built-in CharacterMovementSystem.
     *
     * 엔진 빌트인 CharacterMovementSystem을 사용하기 위한 InputAction 셋.
     * 엔진을 시작할 때 SetCharacterInputActions를 호출하여 설정한다.
     */
    struct CharacterInputActions
    {
        InputAction moveUp{InputAction::None};
        InputAction moveDown{InputAction::None};
        InputAction moveLeft{InputAction::None};
        InputAction moveRight{InputAction::None};
        InputAction jump{InputAction::None};
    };
}

#endif //MANGO_CHARACTERINPUTACTIONS_H