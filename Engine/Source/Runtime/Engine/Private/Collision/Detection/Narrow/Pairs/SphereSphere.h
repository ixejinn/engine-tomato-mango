#ifndef MANGO_SPHERESPHERE_H
#define MANGO_SPHERESPHERE_H

#include "Collision/ContactData.h"
#include "Collision/Detection/Narrow/ShapeView.h"

namespace tomato::SphereSphere
{
    bool Overlap(const ShapeView& a, const ShapeView& b);

    std::optional<ContactData> Sweep(const ShapeView& sphereA, const ShapeView& sphereB, const glm::vec3& ray, bool reversed = false);
}

#endif //MANGO_SPHERESPHERE_H