#pragma once

#include <glm/glm.hpp>

#include "Math/Interval.h"
#include "Math/Ray.h"

class AABB {
public:
	AABB() = default;

	AABB(const Interval& x, const Interval& y, const Interval& z)
		: m_X(x), m_Y(y), m_Z(z) 
	{}

	AABB(const glm::vec3& a, glm::vec3& b);
	AABB(const AABB& box0, const AABB& box1);

	const Interval& AxisInterval(int n) const;

	bool Hit(const Ray& ray, Interval rayInterval) const;

	int LongestAxis() const;

public:
	static const AABB Empty, Universe;

private:
	Interval m_X, m_Y, m_Z;
};