#include "rtpch.h"
#include "Hittable.h"

#include "Math/MathUtil.h"

Translate::Translate(Ref<Hittable> object, const glm::vec3& offset)
    : m_Object(object), m_Offset(offset)
{
    m_BoundingBox = m_Object->BoundingBox() + m_Offset;
}

bool Translate::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
    // Move the ray backwards by the offset
    Ray offsetRay(ray.Origin() - m_Offset, ray.Direction(), ray.time());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (!m_Object->Hit(offsetRay, rayInterval, record))
        return false;

    // Move the intersection point forwards by the offset
    record.Point += m_Offset;

    return true;
}

RotateY::RotateY(Ref<Hittable> object, float angle)
    : m_Object(object)
{
    float radians = MathUtil::DegreeToRadians(angle);
    m_SinTheta = sin(radians);
    m_CosTheta = cos(radians);
    m_BoundingBox = object->BoundingBox();

    glm::vec3 min(INFINITY, INFINITY, INFINITY);
    glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);

    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            for (int k = 0; k < 2; k++) 
            {
                float x = i * m_BoundingBox.max().x + (1 - i) * m_BoundingBox.min().x;
                float y = j * m_BoundingBox.max().y + (1 - j) * m_BoundingBox.min().y;
                float z = k * m_BoundingBox.max().z + (1 - k) * m_BoundingBox.min().z;

                auto newx = m_CosTheta * x + m_SinTheta * z;
                auto newz = -m_SinTheta * x + m_CosTheta * z;

                glm::vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) 
                {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    m_BoundingBox = AABB(min, max);
}

bool RotateY::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
    // Change the ray from world space to object space
    glm::vec3 origin = ray.Origin();
    glm::vec3 direction = ray.Direction();

    origin.x = m_CosTheta * ray.Origin().x - m_SinTheta * ray.Origin().z;
    origin.z = m_SinTheta * ray.Origin().x + m_CosTheta * ray.Origin().z;

    direction.x = m_CosTheta * ray.Direction().x - m_SinTheta * ray.Direction().z;
    direction.z = m_SinTheta * ray.Direction().x + m_CosTheta * ray.Direction().z;

    Ray rotatedRay(origin, direction, ray.time());

    // Determine whether an intersection exists in object space (and if so, where)
    if (!m_Object->Hit(rotatedRay, rayInterval, record)) return false;

    // Change the intersection point from object space to world space
    glm::vec3 point = record.Point;
    point.x = m_CosTheta * record.Point.x + m_SinTheta * record.Point.z;
    point.z = -m_SinTheta * record.Point.x + m_CosTheta * record.Point.z;

    // Change the normal from object space to world space
    glm::vec3 normal = record.Normal;
    normal.x = m_CosTheta * record.Normal.x + m_SinTheta * record.Normal.z;
    normal.x = -m_SinTheta * record.Normal.x + m_CosTheta * record.Normal.z;

    record.Point = point;
    record.Normal = normal;

    return true;
}
