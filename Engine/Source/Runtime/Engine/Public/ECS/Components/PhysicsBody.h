#ifndef MANGO_RIGIDBODY_H
#define MANGO_RIGIDBODY_H

namespace tomato
{
    struct PhysicsBodyComponent
    {
        float inverseMass{1.0f};    // 0: 움직이지 않음
    };
}

#endif //MANGO_RIGIDBODY_H