#ifndef MANGO_GAMEINPUT_H
#define MANGO_GAMEINPUT_H

#include "Input/InputAction.h"
#include "Input/InputMappingContext.h"
#include "GameObject/Character/CharacterInputActions.h"
#include "Engine/TomatoFwd.h"

namespace GameInputAction
{
    using tomato::InputAction;

    inline constexpr InputAction moveUp   {1 << 0};
    inline constexpr InputAction moveDown {1 << 1};
    inline constexpr InputAction moveLeft {1 << 2};
    inline constexpr InputAction moveRight{1 << 3};
    inline constexpr InputAction jump     {1 << 4};
}

namespace GameInputMappingContext
{
    using tomato::Key;

    inline const tomato::InputMappingContext P1{
        .mappings =
        {
            {GameInputAction::moveUp,    Key::W},
            {GameInputAction::moveDown,  Key::S},
            {GameInputAction::moveLeft,  Key::A},
            {GameInputAction::moveRight, Key::D},
            {GameInputAction::jump,      Key::SpaceBar}
        }
    };

    inline const tomato::InputMappingContext P2{
        .mappings =
        {
                {GameInputAction::moveUp,    Key::KP_8},
                {GameInputAction::moveDown,  Key::KP_5},
                {GameInputAction::moveLeft,  Key::KP_4},
                {GameInputAction::moveRight, Key::KP_6},
                {GameInputAction::jump,      Key::KP_0}
        }
    };
}

inline constexpr tomato::CharacterInputActions gameCharacterInputActions
{
    .moveUp = GameInputAction::moveUp,
    .moveDown = GameInputAction::moveDown,
    .moveLeft = GameInputAction::moveLeft,
    .moveRight = GameInputAction::moveRight,
    .jump = GameInputAction::jump
};

void SetDefaultGameInput(tomato::Engine& engine, bool use2P = false);

#endif //MANGO_GAMEINPUT_H