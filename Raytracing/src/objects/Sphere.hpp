#pragma once

#include "objects\Hittable.hpp"
#include "glm\glm.hpp"

class Sphere : public Hittable 
{
public:
    Sphere(glm::vec3 cen, float r, shared_ptr<Material::Blank> m) : center(cen), radius(r), materialPtr(m) {};

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override
    {
        glm::vec3 oc = r.origin() - center;
        float a = r.direction().x * r.direction().x + r.direction().y * r.direction().y + r.direction().z * r.direction().z;
        float half_b = glm::dot(oc, r.direction());
        float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.materialPtr = materialPtr;

        return true;
    }

public:
    glm::vec3 center;
    float radius;
    shared_ptr<Material::Blank> materialPtr;
};
