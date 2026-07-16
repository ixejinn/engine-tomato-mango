#include "State/StateRegistry.h"
#include "State/State.h"
#include "Resource/AssetHash.h"
#include "Utils/Logger.h"

namespace tomato
{
    void StateRegistry::RegisterFactory(const std::string& stateName, StateFactory &&factory)
    {
        const AssetID id = GetAssetID(stateName);

        if (idToFactory_.contains(id))
            TMT_WARN << "Duplicated State name " << idToName_[id] << " detected. Overwriting with the new StateFactory.";
        else
            idToName_[id] = stateName;

        idToFactory_[id] = factory;
    }
}