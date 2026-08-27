#include <iostream>
#include "Profiler/Profiler.h"
#include "Profiler/CPUUsage.h"

namespace tomato
{
    void Profiler::Start()
    {
        frameCnt_ = 0;
    }

    void Profiler::End()
    {
        active_ = false;

        size_t startIdx = frameCnt_ - 256;
        int totalCnt = 256;
        if (frameCnt_ < 256) {
            startIdx = 0;
            totalCnt = frameCnt_;
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

        auto view = registry_.view<CPUUsageGraphDataComponent>();
        for (auto [e, graphData] : view.each())
        {
            long long sum = 0;
            for (size_t i = startIdx; i < frameCnt_; ++i)
                sum += graphData.data[i];

            double avg = sum / totalCnt;

            if (e == main_)
                avgMain = avg;
            else
            {
                std::cout << " " << std::left << std::setw(39) << graphData.name
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
        std::cout << " * Each average is calculated over the last " << totalCnt << " recorded frames.\n";
        std::cout << "================================================================================\n\n";
    }

    void Profiler::BeginFrame()
    {
        auto view = registry_.view<CPUUsageComponent>();
        for (auto [e, usage] : view.each())
        {
            usage.usage = 0;
        }
    }

    void Profiler::Update()
    {
        auto view = registry_.view<CPUUsageComponent, CPUUsageGraphDataComponent>();
        for (auto [e, usage, graphData] : view.each())
        {
            graphData.data[frameCnt_] = usage.usage;
        }

        ++frameCnt_;
    }
}