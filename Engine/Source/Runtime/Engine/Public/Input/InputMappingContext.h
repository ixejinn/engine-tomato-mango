#ifndef MANGO_INPUTMAPPINGCONTEXT_H
#define MANGO_INPUTMAPPINGCONTEXT_H

#include <vector>
#include "Input/KeyConstants.h"
#include "Input/InputAction.h"

namespace tomato
{
    struct ActionKeyMapping
    {
        InputAction action{InputAction::None};
        Key key{Key::UNKNOWN};
    };

    /**
     * @brief A set of key-to-action mappings.
     *
     * 로컬 플레이어마다 하나씩 지정되어야 하며, PlayerInput에 바인딩을 등록하기 위해 사용한다.
     */
    struct InputMappingContext
    {
        std::vector<ActionKeyMapping> mappings;
    };
}

#endif //MANGO_INPUTMAPPINGCONTEXT_H