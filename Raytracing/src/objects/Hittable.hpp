#pragma once

#include "rendering\Ray.hpp"
#include "glm\glm.hpp"

namespace Material 
{
    class Blank;
}

using std::shared_ptr;

struct HitRecord {
    glm::vec3 p;
    glm::vec3 normal;
    shared_ptr<Material::Blank> materialPtr;
    float t;
    bool front_face;

    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const { return 0; };
};