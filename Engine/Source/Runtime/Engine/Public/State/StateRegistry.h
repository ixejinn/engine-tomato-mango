#ifndef MANGO_STATEREGISTRY_H
#define MANGO_STATEREGISTRY_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>
#include "State/StateFwd.h"
#include "Resource/ResourceFwd.h"
#include "TomatoFwd.h"

namespace tomato
{
    using StateFactory = std::function<std::unique_ptr<State>(Engine&)>;

    /**
     * @brief A registry class that manages the mapping between runtime types, stable StateID(AssetID)s, and factory functions for saving and loading States.
     *
     * 각 State 객체가 본인의 정확한 식별자나 문자열을 내부에 저장할 필요가 없도록 하기 위한 C++의 런타임 타입 정보(std::type_index)와
     * 컴파일러에 독립적인 고유 식별자(AssetID 활용)를 연결하여 State의 팩토리 함수를 찾을 수 있다.
     * - Save: State 객체에서 얻을 수 있는 type_index로 등록된 StateID를 찾아 세이브 파일에 기록. (함수 GetStateID 호출)
     * - Load: 세이브 파일에서 AssetID를 읽어 매핑된 팩토리 함수를 호출. (함수 GetStateFactory 호출)
     *
     * @note 매크로를 통해 AssetID가 지정되므로, State 클래스의 이름이 변경될 경우 변경되기 이전의 세이브 파일과 호환되지 않음에 유의.
     */
    class StateRegistry
    {
        StateRegistry() = default;

    public:
        ~StateRegistry() = default;

        StateRegistry(const StateRegistry&) = delete;
        StateRegistry& operator=(const StateRegistry&) = delete;

        static StateRegistry& GetInstance()
        {
            static StateRegistry instance;
            return instance;
        }

        void RegisterFactory(std::type_index type, const std::string& name, StateFactory&& factory);

        AssetID GetStateID(std::type_index type) { return typeToID_[type]; }

        StateFactory& GetStateFactory(std::type_index type) { return IDToFactory_[typeToID_[type]]; }
        StateFactory& GetStateFactory(AssetID id) { return IDToFactory_[id]; }

    private:
        std::unordered_map<std::type_index, AssetID> typeToID_;
        std::unordered_map<AssetID, StateFactory> IDToFactory_;
    };
}

#endif //MANGO_STATEREGISTRY_H
