#ifndef MANGO_INPUTACTION_H
#define MANGO_INPUTACTION_H

#include <cstdint>

namespace tomato {
    /**
     * @brief A logical player action.
     *
     * 각 액션은 비트 하나이며, Contents에서 상수로 정의하여 사용한다.
     * @code
     * inline constexpr InputAction Jump{1 << 4};
     * @endcode
     *
     * 여러 액션을 비트 마스크로 합쳐서 ActionSnapshot에 저장한다.
     * InputMappingContext에서 물리 키와 연결한다.
     */
    enum class InputAction : uint32_t
    {
        None    = 0
    };
}

#endif //MANGO_INPUTACTION_H