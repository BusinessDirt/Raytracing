#include "rtpch.h"
#include "Math/AABB.h"
#include "Math/MathUtil.h"

const AABB AABB::Empty      =   AABB(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe   =   AABB(Interval::Universe, Interval::Universe, Interval::Universe);

AABB::AABB(const Interval& x, const Interval& y, const Interval& z)
    : m_Min(std::numeric_limits<float>::max()), m_Max(std::numeric_limits<float>::min())
{}

AABB::AABB(const glm::vec3& min, glm::vec3& max)
    : m_Min(min), m_Max(max)
{}
    

AABB::AABB(const AABB& left, const AABB& right)
    : m_Min(glm::min(left.min(), right.min())), m_Max(glm::max(left.max(), right.max()))
{}

bool AABB::Hit(const Ray& ray, Interval rayInterval) const
{
    const glm::vec3 invDirection = 1.0f / ray.Direction();
    const glm::vec3 t0 = (m_Min - ray.Origin()) * invDirection;
    const glm::vec3 t1 = (m_Max - ray.Origin()) * invDirection;

    float tMin = std::max(rayInterval.min(), MathUtil::Max(glm::min(t0, t1)));
    float tMax = std::min(rayInterval.max(), MathUtil::Min(glm::max(t0, t1)));

    return tMax > tMin;
}

int AABB::LongestAxis() const
{
    // Returns the index of the longest axis of the bounding box.
    // Calculate the lengths along each axis
    float deltaX = m_Max.x - m_Min.x;
    float deltaY = m_Max.y - m_Min.y;
    float deltaZ = m_Max.z - m_Min.z;

    return (deltaX >= deltaY && deltaX >= deltaZ) ? 0 : (deltaY >= deltaX && deltaY >= deltaZ) ? 1 : 2;
}
