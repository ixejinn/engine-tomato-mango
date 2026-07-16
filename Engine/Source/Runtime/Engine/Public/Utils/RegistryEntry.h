#ifndef MANGO_REGISTRYENTRY_H
#define MANGO_REGISTRYENTRY_H

#include <string>
#include "ECS/SystemRegistry.h"
#include "Resource/ResourceFwd.h"
#include "State/StateRegistry.h"

namespace tomato {
    /**
     * @brief Helper struct used for static registration of factories.
     *
     * 자동 등록을 위한 구조체.
     * 각 시스템 .cpp의 익명 네임스페이스에서 매크로를 사용하여
     * RegistryEntry 정적 객체를 생성하면 팩토리 함수가 Registry에 자동으로 등록된다.
     */
    struct RegistryEntry {
        RegistryEntry(const SystemPhase phase, SystemFactory&& factory) {
            SystemRegistry::GetInstance().RegisterFactory(phase, std::move(factory));
        }

        RegistryEntry(const std::string& stateName, StateFactory&& factory)
        {
            StateRegistry::GetInstance().RegisterFactory(stateName, std::move(factory));
        }
    };
}

#define REGISTER_BUILT_IN_SYSTEM(PHASE, CLASS)\
namespace { static tomato::RegistryEntry CLASS##Entry{PHASE, []() { return std::make_unique<tomato::CLASS>(); }}; }

#define REGISTER_SYSTEM(PHASE, CLASS)\
namespace { static tomato::RegistryEntry CLASS##Entry{PHASE, []() { return std::make_unique<CLASS>(); }}; }

#define REGISTER_BUILT_IN_STATE(CLASS) \
namespace { static tomato::RegistryEntry CLASS##Entry{#CLASS, [](tomato::Engine& engine) { return std::make_unique<tomato::CLASS>(engine); }}; }

#define REGISTER_STATE(CLASS) \
namespace { static tomato::RegistryEntry CLASS##Entry{#CLASS, [](tomato::Engine& engine) { return std::make_unique<CLASS>(engine); }}; }

#endif //MANGO_REGISTRYENTRY_H