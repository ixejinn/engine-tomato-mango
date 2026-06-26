#ifndef MANGO_SYSTEMREGISTRY_H
#define MANGO_SYSTEMREGISTRY_H

#include <functional>
#include <memory>
#include <vector>
#include "Forward/SystemFwd.h"
#include "ECS/SystemConstants.h"
#include "Containers/EnumArray.h"

namespace tomato {
    using SystemFactory = std::function<std::unique_ptr<System>()>;

    /**
     * @brief Global registry that stores system factory functions grouped by SystemPhase.
     *
     * 시스템 생성 함수를 저장하는 싱글톤 클래스.
     * SystemManager는 SystemRegistry를 통해 생성자에서 시스템 객체를 자동으로 생성할 수 있다.
     */
    class SystemRegistry {
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

        void RegisterFactory(SystemPhase phase, SystemFactory&& factory);
        const std::vector<SystemFactory>& GetSimFactory(SystemPhase phase);

    private:
        EnumArray<SystemPhase, std::vector<SystemFactory>> factories_;
    };
}

#endif //MANGO_SYSTEMREGISTRY_H