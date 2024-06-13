#include "rtpch.h"
#include "Objects/Sphere.h"
#include "Math/MathUtil.h"

Sphere::Sphere(const glm::vec3& center, float radius, std::shared_ptr<Material::Blank> material)
    : m_Center(center), m_Radius((float)std::fmax(0, radius)), m_MaterialPtr(material), m_Moving(false)
{
    glm::vec3 radiusVector = glm::vec3(radius, radius, radius);
    m_BoundingBox = AABB(m_Center - radiusVector, m_Center + radiusVector);

    m_CenterVec = glm::vec3(0.0f);
}

Sphere::Sphere(const glm::vec3& center1, const glm::vec3& center2, double radius, std::shared_ptr<Material::Blank> material) 
    : m_Center(center1), m_Radius((float)std::fmax(0, radius)), m_MaterialPtr(material), m_Moving(true) 
{
    glm::vec3 radiusVector = glm::vec3(radius, radius, radius);
    AABB box1(center1 - radiusVector, center1 + radiusVector);
    AABB box2(center2 - radiusVector, center2 + radiusVector);
    m_BoundingBox = AABB(box1, box2);

    m_CenterVec = center2 - center1;
}

bool Sphere::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
    glm::vec3 center = m_Moving ? SphereCenter(ray.time()) : m_Center;
    glm::vec3 oc = center - ray.Origin();
    float a = ray.Direction().x * ray.Direction().x + ray.Direction().y * ray.Direction().y + ray.Direction().z * ray.Direction().z;
    float h = glm::dot(ray.Direction(), oc);
    float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - m_Radius * m_Radius;

    float discriminant = h * h - a * c;
    if (discriminant < 0) return false;
    float sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (h - sqrtd) / a;
    if (!rayInterval.Surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!rayInterval.Surrounds(root)) return false;
    }

    record.Intersection = root;
    record.Point = ray.At(record.Intersection);
    glm::vec3 outwardNormal = (record.Point - center) / m_Radius;
    record.SetFaceNormal(ray, outwardNormal);
    GetSphereUV(outwardNormal, record.U, record.V);
    record.MaterialPtr = m_MaterialPtr;

    return true;
}


void Sphere::GetSphereUV(const glm::vec3& point, float& u, float& v)
{
    float theta = std::acos(-point.y);
    float phi = std::atan2(-point.z, point.x) + PI;

    u = phi / (2.0f * PI);
    v = theta / PI;
}
