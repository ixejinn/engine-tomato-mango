#ifndef MANGO_STATEREGISTRY_H
#define MANGO_STATEREGISTRY_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "State/StateFwd.h"
#include "TomatoFwd.h"

namespace tomato
{
    using StateFactory = std::function<std::unique_ptr<State>(Engine&)>;

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

        void RegisterFactory(std::type_index type, StateFactory&& factory);

        StateFactory& GetStateFactory(std::type_index type) { return typeToFactory_[type]; }

    private:
        std::unordered_map<std::type_index, StateFactory> typeToFactory_;
    };
}

#endif //MANGO_STATEREGISTRY_H
