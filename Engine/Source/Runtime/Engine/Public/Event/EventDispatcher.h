#ifndef MANGO_EVENTDISPATCHER_H
#define MANGO_EVENTDISPATCHER_H

#include <memory>
#include <entt/entt.hpp>

namespace tomato {
    class EventDispatcher {
        EventDispatcher() {
            dispatcher_ = std::make_unique<entt::dispatcher>();
        }

    public:
        ~EventDispatcher() = default;

        static EventDispatcher& GetInstance()
        {
            static EventDispatcher instance;
            return instance;
        }

        template<typename E, auto Candidate>
        void Connect()
        {
            dispatcher_->sink<E>().template connect<Candidate>();
        }

        template<typename E, auto Candidate, typename T>
        void Connect(T& instance)
        {
            dispatcher_->sink<E>().template connect<Candidate>(instance);
        }

        template<typename E>
        void Enqueue(const E& e)
        {
            dispatcher_->enqueue(e);
        }

        template<typename E>
        void Trigger(const E& e)
        {
            dispatcher_->trigger(e);
        }

        void Update()
        {
            dispatcher_->update();
        }

        template<typename E, auto Candidate>
        void Disconnect()
        {
            dispatcher_->sink<E>().template disconnect<Candidate>();
        }

        template<typename E, auto Candidate, typename T>
        void Disconnect(T& instance)
        {
            dispatcher_->sink<E>().template disconnect<Candidate>(instance);
        }

        void ClearEvents()
        {
            dispatcher_->clear();
        }

        void Clear()
        {
            dispatcher_ = std::make_unique<entt::dispatcher>();
        }

    private:
        std::unique_ptr<entt::dispatcher> dispatcher_;
    };
}

#endif //MANGO_EVENTDISPATCHER_H