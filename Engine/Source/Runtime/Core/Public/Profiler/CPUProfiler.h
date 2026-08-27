#ifndef MANGO_CPUPROFILER_H
#define MANGO_CPUPROFILER_H

#include "entt/entt.hpp"
#include "Utils/StringLiteral.h"
#include "Profiler/Profiler.h"
#include "Profiler/ExecutionTime.h"
#include "EngineConfig.h"

namespace tomato
{
    template<StringLiteral Str>
    struct CPUProfiler
    {
        static void Begin()
        {
            auto& profiler = Profiler::GetInstance();
            auto& registry = profiler.GetRegistry();

            if (e == entt::null)
            {
                // Create CPU profiler entity
                e = registry.create();
                registry.emplace<ExecutionTimeComponent>(e);
                auto& graphData = registry.emplace<ExecutionTimeHistoryComponent>(e);
                graphData.name = Str.value;

                if constexpr (Str == "TOTAL")
                    registry.emplace<TotalFrameTag>(e);
            }

            if (!profiler.IsActive())
                return;

            auto& sample = profiler.GetRegistry().get<ExecutionTimeComponent>(e);
            sample.start = std::chrono::high_resolution_clock::now();
        }

        static void End()
        {
            auto& profiler = Profiler::GetInstance();
            if (!profiler.IsActive())
                return;

            auto& sample = profiler.GetRegistry().get<ExecutionTimeComponent>(e);

            auto end = std::chrono::high_resolution_clock::now();
            sample.microSecs = std::chrono::duration_cast<std::chrono::microseconds>(end - sample.start).count();
        }

        inline static entt::entity e{entt::null};
    };
}

#ifdef TOMATO_DEBUG
#define CPU_PROFILER_BLOCK_BEGIN(BLOCK_NAME) tomato::CPUProfiler<#BLOCK_NAME>::Begin();
#define CPU_PROFILER_BLOCK_END(BLOCK_NAME)   tomato::CPUProfiler<#BLOCK_NAME>::End();
#define CPU_PROFILER_TOTAL_BEGIN() tomato::CPUProfiler<"TOTAL">::Begin();
#define CPU_PROFILER_TOTAL_END()   tomato::CPUProfiler<"TOTAL">::End();
#else
#define CPU_PROFILER_BLOCK_BEGIN(BLOCK_NAME)
#define CPU_PROFILER_BLOCK_END(BLOCK_NAME)
#define CPU_PROFILER_TOTAL_BEGIN()
#define CPU_PROFILER_TOTAL_END()
#endif

#endif //MANGO_CPUPROFILER_H
