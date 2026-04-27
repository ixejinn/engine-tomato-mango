#ifndef MANGO_ATLASMANAGER_H
#define MANGO_ATLASMANAGER_H

#include <memory>
#include <vector>
#include "Font/TextureAtlas.h"

namespace tomato {
    struct AllocationResult
    {
        int atlasIndex;
        int x, y;
    };

    class AtlasManager {
    public:
        AllocationResult RequestAllocate(int w, int h);
        void BindAtlas(int idx, unsigned int unit = 0);

        TextureAtlas* GetAtlas(int idx) { return atlases_[idx].get(); }

    private:
        /**
         * Add a new 2048x2048 atlas to the pool.
         */
        void CreateNewAtlas();

        std::vector<std::unique_ptr<TextureAtlas>> atlases_;
    };
}

#endif //MANGO_ATLASMANAGER_H