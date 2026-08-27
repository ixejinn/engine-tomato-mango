#ifndef MANGO_CPUPROFILER_H
#define MANGO_CPUPROFILER_H

#include "entt/entt.hpp"
#include "Utils/StringLiteral.h"
#include "Profiler/Profiler.h"
#include "Profiler/CPUUsage.h"

namespace tomato
{
    template<StringLiteral Str>
    struct CPUProfiler
    {
        static void MarkerBegin()
        {
            if (e == entt::null)
            {
                auto& profiler = Profiler::GetInstance();
                auto& registry = profiler.GetRegistry();
                e = registry.create();
                registry.emplace<CPUUsageComponent>(e);
                auto& graphData = registry.emplace<CPUUsageGraphDataComponent>(e);
                graphData.name = std::string(Str);

                if constexpr (Str == "TOTAL")
                    profiler.SetMainProfiler(e);
            }

            auto& profiler = Profiler::GetInstance();
            if (!profiler.IsActive())
                return;
            
            auto& usage = profiler.GetRegistry().get<CPUUsageComponent>(e);
            usage.start = std::chrono::high_resolution_clock::now();
        }

        static void MarkerEnd()
        {
            auto& profiler = Profiler::GetInstance();
            if (!profiler.IsActive())
                return;

            auto& usage = profiler.GetRegistry().get<CPUUsageComponent>(e);

            auto end = std::chrono::high_resolution_clock::now();
            usage.usage = std::chrono::duration_cast<std::chrono::microseconds>(end - usage.start).count();
        }

        inline static entt::entity e{entt::null};
    };
}

#define CPU_PROFILER_BEGIN(BLOCK_NAME) tomato::CPUProfiler<#BLOCK_NAME>::MarkerBegin();
#define CPU_PROFILER_END(BLOCK_NAME)   tomato::CPUProfiler<#BLOCK_NAME>::MarkerEnd();

#endif //MANGO_CPUPROFILER_H
