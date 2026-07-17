#include "State/StateRegistry.h"
#include "Resource/AssetHash.h"
#include "Utils/Logger.h"

namespace tomato
{
    void StateRegistry::RegisterFactory(const std::type_index type, const std::string& name, StateFactory&& factory)
    {
        if (typeToID_.contains(type))
            TMT_WARN << "Duplicated State name " << type.name() << " detected. Overwriting with the new StateFactory.";
        else
            typeToID_[type] = GetAssetID(name);

        IDToFactory_[typeToID_[type]] = factory;
        std::cout << type.name() << ": " << GetAssetID(name) << '\n';
    }
}