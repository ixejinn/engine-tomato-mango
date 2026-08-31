#ifndef MANGO_CPUUSAGE_H
#define MANGO_CPUUSAGE_H

#include <chrono>
#include <string>
#include "Containers/RingArray.h"
#include "Profiler/ProfilerConfig.h"

namespace tomato
{
    struct ExecutionTimeComponent
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
        int microSecs{0};
    };

    struct ExecutionTimeHistoryComponent
    {
        std::string_view name;
        RingArray<int, PROFILER_SAMPLE_COUNT> data;
    };

    struct TotalFrameTag {};
}

#endif //MANGO_CPUUSAGE_H
