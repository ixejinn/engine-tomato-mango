#ifndef MANGO_PROFILER_H
#define MANGO_PROFILER_H

#include "entt/entt.hpp"

namespace tomato
{
    class Profiler
    {
        Profiler() = default;

    public:
        ~Profiler() = default;

        Profiler(const Profiler&) = delete;
        Profiler& operator=(const Profiler&) = delete;

        static Profiler& GetInstance()
        {
            static Profiler instance;
            return instance;
        }

        entt::registry& GetRegistry() { return registry_; }
        size_t GetFrameCnt() const { return frameCnt_; }

        bool IsActive() const { return active_; }
        void SetActive(bool active) { active_ = active; }
        void ToggleActive() { active_ = !active_; }

        void Start();
        void End();

        void BeginFrame();
        void Update();

    private:
        entt::registry registry_;

        size_t frameCnt_{0};
        bool active_{false};
    };
}

#endif //MANGO_PROFILER_H
