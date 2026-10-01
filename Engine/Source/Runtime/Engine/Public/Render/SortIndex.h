#ifndef MANGO_SORTINDEX_H
#define MANGO_SORTINDEX_H

#include <cstdint>

namespace tomato
{
    template <typename T>
    class SortIndexed
    {
    public:
        uint16_t GetSortIndex() const { return sortIndex_; }

    protected:
        SortIndexed() : sortIndex_(nextSortIndex_++) {}

        SortIndexed(const SortIndexed&) = delete;
        SortIndexed& operator=(const SortIndexed&) = delete;

    private:
        inline static uint16_t nextSortIndex_{0};
        const uint16_t sortIndex_;
    };
}

#endif //MANGO_SORTINDEX_H