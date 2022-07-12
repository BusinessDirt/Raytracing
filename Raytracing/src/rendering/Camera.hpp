#pragma once

#include "glm/glm.hpp"
#include "Ray.hpp"
#include "Util.hpp"

class Camera
{
public:
    Camera() {}

	Camera(uint32_t width, uint32_t height)
	{
        this->Resize(width, height);
	}

    Ray getRay(float u, float v) const {
        return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

    void Resize(uint32_t width, uint32_t height) {
        float aspect_ratio = (float)width / (float)height;
        float viewport_height = 2.0;
        float viewport_width = aspect_ratio * viewport_height;
        float focal_length = 1.0;

        origin = glm::vec3(0, 0, 0);
        horizontal = glm::vec3(viewport_width, 0.0, 0.0);
        vertical = glm::vec3(0.0, viewport_height, 0.0);
        lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - glm::vec3(0, 0, focal_length);
    }

private:
    glm::vec3 origin;
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
};