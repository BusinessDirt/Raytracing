#pragma once

#include "glm/glm.hpp"

#include "Math/Ray.h"

class Camera
{
public:
    Camera() = default;
    Camera(glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 vUp, uint32_t width, uint32_t height, float vfov, float defocusAngle, float focusDistance);

    Ray GetRay(float s, float t) const;

    void Resize(uint32_t width, uint32_t height);

private:
    glm::vec3 DefocusDiskSample() const;

private:
    glm::vec3 m_Center, m_Pixel00Location;
    glm::vec3 m_PixelDeltaU, m_PixelDeltaV;
    glm::vec3 m_U, m_V, m_W;
    glm::vec3 m_DefocusDiskU, m_DefocusDiskV;

    float m_DefocusAngle, m_FOV, m_FocusDistance;
};