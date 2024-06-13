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

struct HitRecord 
{
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

class Hittable 
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const = 0;

    virtual AABB BoundingBox() const = 0;
};

class Translate : public Hittable
{
public:
    Translate(Ref<Hittable> object, const glm::vec3& offset);

    virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

    virtual AABB BoundingBox() const override { return m_BoundingBox; }

private:
    Ref<Hittable> m_Object;
    glm::vec3 m_Offset;
    AABB m_BoundingBox;
};

class RotateY : public Hittable
{
public:
    RotateY(Ref<Hittable> object, float angle);

    virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

    virtual AABB BoundingBox() const override { return m_BoundingBox; }

private:
    Ref<Hittable> m_Object;
    float m_SinTheta, m_CosTheta;
    AABB m_BoundingBox;
};