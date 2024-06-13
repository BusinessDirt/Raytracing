#pragma once

#include <math.h>
#include <glm/glm.hpp>

class Ray {
public:
	Ray() = default;

	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: m_Origin(origin), m_Direction(direction), m_Time(0.0f)
	{}

	Ray(const glm::vec3& origin, const glm::vec3& direction, float time)
		: m_Origin(origin), m_Direction(direction), m_Time(time)
	{}

	glm::vec3 At(float t) const { return m_Origin + t * m_Direction; }

	const glm::vec3& Direction() const { return m_Direction; }
	const glm::vec3& Origin() const { return m_Origin; }
	float time() const { return m_Time; }

private:
	glm::vec3 m_Origin;
	glm::vec3 m_Direction;
	float m_Time;
};