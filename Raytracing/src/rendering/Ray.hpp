#pragma once

#include <math.h>
#include <glm/glm.hpp>

class Ray {
public:
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: orig(origin), dir(direction) 
	{}

	glm::vec3 at(float t) const {
		return orig + t * dir;
	}

	glm::vec3 direction() {
		return this->dir;
	}

	glm::vec3 origin() {
		return this->orig;
	}

public:
	glm::vec3 orig;
	glm::vec3 dir;
};