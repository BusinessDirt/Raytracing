#pragma once

#include "glm/glm.hpp"
#include "Ray.hpp"
#include "Util.hpp"

class Camera
{
public:
    Camera() {}

    Camera(glm::vec3 lf, glm::vec3 lt, glm::vec3 vup, uint32_t width, uint32_t height, float vfov, float aperture, float focusDistance)
	{
        float theta = DegreeToRadians(vfov);
        float h = tan(theta / 2.0f);
        float aspect_ratio = (float)width / (float)height;
        float viewport_height = 2.0 * h;
        float viewport_width = aspect_ratio * viewport_height;
        
        w = glm::normalize(lf - lt);
        u = glm::normalize(glm::cross(vup, w));
        v = glm::cross(w, u);

        origin = lf;
        horizontal = focusDistance * viewport_width * u;
        vertical = focusDistance * viewport_height * v;
        lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - focusDistance * w;

        lensRadius = aperture / 2.0f;
	}

    Ray GetRay(float s, float t) const 
    {
        glm::vec2 rd = lensRadius * RandomVec2InUnitCircle();
        glm::vec3 offset = u * rd.x + v * rd.y;

        return Ray(
            origin + offset, 
            lower_left_corner + s * horizontal + t * vertical - origin - offset
        );
    }

    void Resize(uint32_t width, uint32_t height) 
    {
        float aspectRatio = (float)width / (float)height;
        float viewportHeight = vertical.y;
        float viewportWidth = aspectRatio * viewportHeight;
        float focalLength = 1.0f;

        horizontal = glm::vec3(viewportWidth, 0.0, 0.0);
        vertical = glm::vec3(0.0, viewportHeight, 0.0);
        lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - glm::vec3(0, 0, focalLength);
    }

private:
    glm::vec3 origin;
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 u, v, w;
    float lensRadius;
};