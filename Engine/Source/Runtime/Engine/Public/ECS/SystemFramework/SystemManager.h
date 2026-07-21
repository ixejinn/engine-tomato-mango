#ifndef MANGO_SYSTEMMANAGER_H
#define MANGO_SYSTEMMANAGER_H

#include <vector>
#include <memory>
#include "Containers/EnumArray.h"
#include "../Forward/SystemFwd.h"
#include "SystemConstants.h"

namespace tomato
{
    /**
     * @brief Owns and updates all registered ECS systems.
     *
     * 시스템들을 틱(고정 주기)과 프레임 주기에 맞춰 업데이트하며,
     * 현재 엔진의 실행 모드에 맞는 시스템의 Update 함수를 호출한다.
     */
    class SystemManager
    {
    public:
        using SystemPtr = std::unique_ptr<System>;

        SystemManager() = default;
        ~SystemManager();

        void AddSystem(TickPhase  phase, RunMode mode, SystemPtr system);
        void AddSystem(FramePhase phase, RunMode mode, SystemPtr system);

        void FixedUpdate(SimContext& simCtx, RunMode mode);
        void Update     (SimContext& simCtx, RunMode mode);

        void Update(TickPhase  phase, SimContext& simCtx, RunMode mode);
        void Update(FramePhase phase, SimContext& simCtx, RunMode mode);

    private:
        struct SystemTrait
        {
            RunMode mode;
            SystemPtr ptr;
        };

        EnumArray<TickPhase,  std::vector<SystemTrait>> tickSystems_;
        EnumArray<FramePhase, std::vector<SystemTrait>> frameSystems_;
    };
}

#endif //MANGO_SYSTEMMANAGER_H