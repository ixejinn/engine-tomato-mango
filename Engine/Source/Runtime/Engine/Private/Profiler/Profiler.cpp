#include <iostream>
#include "Profiler/Profiler.h"
#include "Profiler/ExecutionTime.h"
#include "Services/Window.h"

namespace tomato
{
    void Profiler::Start()
    {
        active_ = true;
        frameCnt_ = 0;
        Window::SetVSync(false);
    }

    void Profiler::End()
    {
        active_ = false;
        Window::SetVSync(true);

        size_t offset = frameCnt_ - PROFILER_SAMPLE_COUNT;
        int sampleCnt = PROFILER_SAMPLE_COUNT;
        if (frameCnt_ < PROFILER_SAMPLE_COUNT) {
            offset = 0;
            sampleCnt = frameCnt_;
        }

        std::cout << "\n";
        std::cout << "================================================================================\n";
        std::cout << "                          PROFILER SUMMARY REPORT\n";
        std::cout << "================================================================================\n";
        std::cout << std::left << std::setw(40) << "Profile block name"
                  << " | " << std::right << std::setw(17) << "Avg CPU Time (us)"
                  << " | " << std::right << std::setw(17) << "Avg CPU Time (ms)\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        double avgMain = -1;

        auto view = registry_.view<ExecutionTimeHistoryComponent>();
        for (auto [e, exeHistory] : view.each())
        {
            long long exeSum = 0;
            for (size_t i = offset; i < frameCnt_; ++i)
                exeSum += exeHistory.data[i];

            double avg = exeSum / sampleCnt;

            if (registry_.all_of<TotalFrameTag>(e))
                avgMain = avg;
            else
            {
                std::cout << " " << std::left << std::setw(39) << exeHistory.name
                    << " | " << std::right << std::fixed << std::setprecision(1) << std::setw(17) << avg
                    << " | " << std::right << std::fixed << std::setprecision(3) << std::setw(17) << avg * 0.001
                    << "\n";
            }
        }

        std::cout << "-----------------------------------------+-------------------+------------------\n";
        std::cout << " " << std::left << std::setw(39) << "Total CPU time"
            << " | " << std::right << std::fixed << std::setprecision(1) << std::setw(17) << avgMain
            << " | " << std::right << std::fixed << std::setprecision(3) << std::setw(17) << avgMain * 0.001
            << "\n";
        
        std::cout << "================================================================================\n";
        std::cout << " * Each average is calculated over the last " << sampleCnt << " recorded frames.\n";
        std::cout << "================================================================================\n\n";
    }

    void Profiler::BeginFrame()
    {
        // Initialize execution time 0
        auto view = registry_.view<ExecutionTimeComponent>();
        for (auto [e, exeTime] : view.each())
        {
            exeTime.microSecs = 0;
        }
    }

    void Profiler::Update()
    {
        // Copy execution time sample of ExecutionTimeComponent to ExecutionTimeHistoryComponent
        auto view = registry_.view<ExecutionTimeComponent, ExecutionTimeHistoryComponent>();
        for (auto [e, exeTime, exeHistory] : view.each())
        {
            exeHistory.data[frameCnt_] = exeTime.microSecs;
        }

        ++frameCnt_;
    }
}