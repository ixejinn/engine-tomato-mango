#ifndef MANGO_SYSTEMMANAGER_H
#define MANGO_SYSTEMMANAGER_H

#include <vector>
#include <memory>
#include "Containers/EnumArray.h"
#include "ECS/SystemFwd.h"
#include "ECS/SystemConstants.h"

namespace tomato {
    /**
     * @brief Manages lifecycle, execution, and update order of all systems.
     *
     * 시스템 객체를 생성하고 소유하며, 각 시스템의 업데이트 함수를 호출해 컴포넌트 갱신을 수행하는 클래스.
     * 시스템의 업데이트 순서를 결정한다.
     */
    class SystemManager {
    public:
        SystemManager();
        ~SystemManager();

        void Simulate(SimContext& sim, InputContext& input);
        void Render(SimContext& sim, RenderContext& render);

    private:
        using SystemPtr = std::unique_ptr<System>;

        static constexpr SystemPhase simOrder_[] = {
            SystemPhase::Input,
            SystemPhase::Physics,
            SystemPhase::Collision,
            SystemPhase::Transformation
        };

        static constexpr SystemPhase renderOrder_[] = {
            SystemPhase::Camera,
            SystemPhase::Rendering
        };

        EnumArray<SystemPhase, std::vector<SystemPtr>> systems_;
    };
}

#endif //MANGO_SYSTEMMANAGER_H