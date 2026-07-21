#ifndef MANGO_SYSTEMREGISTRY_H
#define MANGO_SYSTEMREGISTRY_H

#include <functional>
#include <memory>
#include <vector>
#include "ECS/Forward/SystemFwd.h"
#include "ECS/Forward/SystemFrameworkFwd.h"
#include "ECS/SystemFramework/SystemConstants.h"
#include "Containers/EnumArray.h"

namespace tomato
{
    using SystemFactory = std::function<std::unique_ptr<System>()>;

    /**
     * @brief Manages the registration, factory storage, and initialization of all ECS systems.
     *
     * SystemManager에 시스템 객체를 등록하는 순서를 명시적으로 제어하여 정확한 시스템 업데이트 순서를 보장하는 싱글톤 클래스.
     */
    class SystemRegistry
    {
        SystemRegistry() = default;

    public:
        ~SystemRegistry() = default;

        SystemRegistry(const SystemRegistry&) = delete;
        SystemRegistry& operator=(const SystemRegistry&) = delete;

        static SystemRegistry& GetInstance()
        {
            static SystemRegistry instance;
            return instance;
        }

        void RegisterSystems(SystemManager& manager);

        void RegisterFactory(TickPhase  phase, RunMode mode, SystemFactory&& factory);
        void RegisterFactory(FramePhase phase, RunMode mode, SystemFactory&& factory);

    private:
        struct SystemTrait
        {
            RunMode mode;
            SystemFactory factory;
        };

        EnumArray<TickPhase,  std::vector<SystemTrait>> tickFactories_{};
        EnumArray<FramePhase, std::vector<SystemTrait>> frameFactories_{};
    };
}

#endif //MANGO_SYSTEMREGISTRY_H