#ifndef MANGO_OBJECT_H
#define MANGO_OBJECT_H

#include <entt/entt.hpp>

namespace tomato {
    class Object {
    public:
        Object(entt::registry& reg);
        virtual ~Object() = default;

        entt::entity GetEntity() const { return entity_; }

    protected:
        entt::entity entity_{entt::null};
    };
}

#endif //MANGO_OBJECT_H