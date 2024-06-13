#include "AABB.h"

const AABB AABB::Empty      =   AABB(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe   =   AABB(Interval::Universe, Interval::Universe, Interval::Universe);

AABB::AABB(const glm::vec3& a, glm::vec3& b)
{
    // Treat the two points a and b as extrema for the bounding box, so we don't require a
    // particular minimum/maximum coordinate order.

    m_X = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
    m_Y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
    m_Z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
}

AABB::AABB(const AABB& box0, const AABB& box1)
{
    m_X = Interval(box0.m_X, box1.m_X);
    m_Y = Interval(box0.m_Y, box1.m_Y);
    m_Z = Interval(box0.m_Z, box1.m_Z);
}

const Interval& AABB::AxisInterval(int n) const
{
    if (n == 1) return m_Y;
    if (n == 2) return m_Z;
    return m_X;
}

bool AABB::Hit(const Ray& ray, Interval rayInterval) const
{
    const glm::vec3& rayOrigin = ray.Origin();
    const glm::vec3& rayDirection = ray.Direction();

    for (int axis = 0; axis < 3; axis++) {
        const Interval& axisInterval = AxisInterval(axis);
        const float axisDirectionInverse = 1.0f / rayDirection[axis];

        float t0 = (axisInterval.min() - rayOrigin[axis]) * axisDirectionInverse;
        float t1 = (axisInterval.max() - rayOrigin[axis]) * axisDirectionInverse;

        if (t0 < t1) 
        {
            if (t0 > rayInterval.min()) rayInterval.min(t0);
            if (t1 < rayInterval.max()) rayInterval.max(t1);
        }
        else 
        {
            if (t1 > rayInterval.min()) rayInterval.min(t1);
            if (t0 < rayInterval.max()) rayInterval.max(t0);
        }

        if (rayInterval.max() <= rayInterval.min())
            return false;
    }

    return true;
}

int AABB::LongestAxis() const
{
    if (m_X.Size() > m_Y.Size())
        return m_X.Size() > m_Z.Size() ? 0 : 2;
    else
        return m_Y.Size() > m_Z.Size() ? 1 : 2;
}
