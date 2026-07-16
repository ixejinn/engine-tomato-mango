#include "State/StateRegistry.h"
#include "Utils/Logger.h"

namespace tomato
{
    void StateRegistry::RegisterFactory(const std::type_index type, StateFactory&& factory)
    {
        if (typeToFactory_.contains(type))
            TMT_WARN << "Duplicated State name " << type.name() << " detected. Overwriting with the new StateFactory.";

        typeToFactory_[type] = factory;
    }
}