#ifndef MANGO_CHARACTERMOVEMENTSYSTEM_H
#define MANGO_CHARACTERMOVEMENTSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    /**
     * @brief Kinematic movement system that updates entity positions by direct coordinate calculation.
     *
     * 물리 시뮬레이션 없이 좌표 계산으로 이동을 처리하는 운동학적 이동 시스템.
     * 점프는 단순한 물리 시뮬레이션을 사용한다.
     */
    class CharacterMovementSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_CHARACTERMOVEMENTSYSTEM_H