#ifndef MANGO_STATEREGISTRY_H
#define MANGO_STATEREGISTRY_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include "Resource/ResourceFwd.h"
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

        void RegisterFactory(const std::string& stateName, StateFactory&& factory);

        StateFactory& GetStateFactory(AssetID id) { return idToFactory_[id]; }
        const std::string& GetStateName(AssetID id) { return idToName_[id]; }

    private:
        std::unordered_map<AssetID, StateFactory> idToFactory_;
        std::unordered_map<AssetID, std::string> idToName_;
    };
}

#endif //MANGO_STATEREGISTRY_H
