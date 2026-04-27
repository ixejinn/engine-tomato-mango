#ifndef MANGO_EVENTSIGNAL_H
#define MANGO_EVENTSIGNAL_H

#include <entt/entt.hpp>

namespace tomato {
    template<typename EventT>
    class EventSignal {
    public:
        template<auto Candidate>
        void Connect()
        {
            entt::sink sink{signal_};
            sink.template connect<Candidate>();
        }

        template<auto Candidate, typename T>
        void Connect(T& instance)
        {
            entt::sink sink{signal_};
            sink.template connect<Candidate>(instance);
        }

        template<typename... Args>
        void Publish(Args&&... args)
        {
            signal_.publish(args...);
        }

        template<auto Candidate>
        void Disconnect()
        {
            entt::sink sink{signal_};
            sink.template disconnect<Candidate>();
        }

        template<auto Candidate, typename T>
        void Disconnect(T& instance)
        {
            entt::sink sink{signal_};
            sink.template disconnect<Candidate>(instance);
        }

        template<typename CollectorT>
        void Collect(CollectorT& collector, const EventT& event)
        {
            signal_.collect(collector, event);
        }

    private:
        entt::sigh<bool(const EventT&)> signal_;
    };
}

#endif //MANGO_EVENTSIGNAL_H