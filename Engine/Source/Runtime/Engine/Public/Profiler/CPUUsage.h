#ifndef MANGO_CPUUSAGE_H
#define MANGO_CPUUSAGE_H

#include <chrono>
#include <string>
#include "Containers/RingArray.h"

namespace tomato
{
    struct CPUUsageComponent
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> start;

        int usage;
    };

    struct CPUUsageGraphDataComponent
    {
        std::string name;
        RingArray<int, 256> data;
    };
}

#endif //MANGO_CPUUSAGE_H
