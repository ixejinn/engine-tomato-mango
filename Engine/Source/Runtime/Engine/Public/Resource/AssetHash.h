#ifndef MANGO_ASSETHASH_H
#define MANGO_ASSETHASH_H

#include "Resource/ResourceFwd.h"

namespace tomato {
    constexpr AssetID FNV1a64B(const char* str) {
        uint64_t hash = 14695981039346656037ULL;
        while (*str) {
            hash ^= static_cast<uint64_t>(*str);
            hash *= 1099511628211ULL;
            ++str;
        }

        return hash;
    }

    constexpr AssetID GetAssetID(const char* name) {
        return FNV1a64B(name);
    }
}

#endif //MANGO_ASSETHASH_H