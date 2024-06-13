#pragma once

#include "Objects\Hittable.h"

#include "glm\glm.hpp"

class Sphere : public Hittable 
{
public:
    Sphere(const glm::vec3& center, float radius, std::shared_ptr<Material::Blank> material);
    Sphere(const glm::vec3& center1, const glm::vec3& center2, double radius, std::shared_ptr<Material::Blank> material);

    bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

    AABB BoundingBox() const override { return m_BoundingBox; }

private:
    glm::vec3 SphereCenter(float time) const { return m_Center + time * m_CenterVec; }

    /**
    * Maps texture uv coordinates to a sphere
    * <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    * <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    * <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    * 
    * @param point a given point on the sphere of radius one, centered at the origin.
    * @param u returned value [0,1] of angle around the Y axis from X=-1.
    * @param v returned value [0,1] of angle from Y=-1 to Y=+1.
    */
    static void GetSphereUV(const glm::vec3& point, float& u, float& v);

private:
    glm::vec3 m_Center;
    float m_Radius;
    std::shared_ptr<Material::Blank> m_MaterialPtr;

    bool m_Moving;
    glm::vec3 m_CenterVec;

    AABB m_BoundingBox;
};
