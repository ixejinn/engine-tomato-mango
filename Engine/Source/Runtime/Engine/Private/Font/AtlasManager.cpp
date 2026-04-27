#include "Font/AtlasManager.h"
#include "Utils/Logger.h"

namespace tomato {
    AllocationResult AtlasManager::RequestAllocate(int w, int h) {
        // 1. If no atlas exists, create the first one
        if (atlases_.empty())
            CreateNewAtlas();

        // 2. Check ONLY the last atlas in the list
        int x, y;
        if (atlases_.back()->Allocate(w, h, x, y))
            return { static_cast<int>(atlases_.size() - 1), x, y };

        // 3. If the last one is full, create a new one and allocate there
        CreateNewAtlas();
        atlases_.back()->Allocate(w, h, x, y);

        return { static_cast<int>(atlases_.size() - 1), x, y };
    }

    void AtlasManager::BindAtlas(int idx, unsigned int unit) {
        if (idx >= 0 && idx < static_cast<int>(atlases_.size()))
            atlases_[idx]->Bind(unit);
        else
            TMT_ERR << "Invalid Atlas Index: " << idx;
    }

    void AtlasManager::CreateNewAtlas()
    {
        atlases_.push_back(std::make_unique<TextureAtlas>());
        TMT_DEBUG << "New TextureAtlas created. Current Total: " << atlases_.size();
    }
}