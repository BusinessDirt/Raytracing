#pragma once

#include <memory>

#include "glm/glm.hpp"

#include "Math/Ray.h"
#include "Math/Interval.h"
#include "Math/AABB.h"

namespace Material 
{
    class Blank;
}

struct HitRecord {
    glm::vec3 Point;
    glm::vec3 Normal;
    std::shared_ptr<Material::Blank> MaterialPtr;
    float Intersection;
    float U, V;
    bool FrontFace;

    inline void SetFaceNormal(const Ray& ray, const glm::vec3& outwardNormal) 
    {
        FrontFace = glm::dot(ray.Direction(), outwardNormal) < 0;
        Normal = FrontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const = 0;

    virtual AABB BoundingBox() const = 0;
};