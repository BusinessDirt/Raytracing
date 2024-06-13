#pragma once

#include <glm/glm.hpp>

#include "Math/Interval.h"
#include "Math/Ray.h"

class AABB {
public:
	AABB() = default;
	AABB(const Interval& x, const Interval& y, const Interval& z);
	AABB(const glm::vec3& min, glm::vec3& max);
	AABB(const AABB& left, const AABB& right);

	bool Hit(const Ray& ray, Interval rayInterval) const;

	int LongestAxis() const;

	inline glm::vec3 min() const { return m_Min; }
	inline glm::vec3 max() const { return m_Max; }

public:

	static const AABB Empty, Universe;

private:
	glm::vec3 m_Min, m_Max;
};

inline AABB operator+(const AABB& bbox, const glm::vec3& offset) {
	return AABB(bbox.min() + offset, bbox.max() + offset);
}

inline AABB operator+(const glm::vec3& offset, const AABB& bbox) {
	return bbox + offset;
}