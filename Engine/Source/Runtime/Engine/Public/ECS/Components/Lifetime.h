#ifndef MANGO_LIFETIME_H
#define MANGO_LIFETIME_H

#include <optional>
#include <cstdint>

namespace tomato
{
//@TODO : Register Component
    struct LifetimeComponent
    {
        std::optional<int64_t> constructed{std::nullopt};
        std::optional<int64_t> destructed{std::nullopt};

        std::optional<int64_t> inactivated{std::nullopt};
        bool isActive{true};
    };
}

#endif //MANGO_LIFETIME_H