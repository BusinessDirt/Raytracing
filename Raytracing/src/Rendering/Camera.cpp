#include "rtpch.h"
#include "Rendering/Camera.h"

#include "Math/MathUtil.h"

Camera::Camera(glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 vUp, uint32_t width, uint32_t height, float vfov, float defocusAngle, float focusDistance)
    : m_Center(lookFrom), m_FOV(vfov), m_DefocusAngle(defocusAngle), m_FocusDistance(focusDistance)
{
    SetDirection(lookFrom, lookAt, vUp, vfov);
    SetFocus(defocusAngle, focusDistance);
    Resize(width, height);
}

Ray Camera::GetRay(int i, int j) const
{
    glm::vec3 offset = MathUtil::SampleSquare();
    glm::vec3 pixelSample = m_Pixel00Location
        + (i + offset.x) * m_PixelDeltaU
        + (j + offset.y) * m_PixelDeltaV;

    glm::vec3 rayOrigin = (m_DefocusAngle <= 0) ? m_Center : DefocusDiskSample();
    glm::vec3 rayDirection = pixelSample - rayOrigin;
    float rayTime = Walnut::Random::Float();

    return Ray(rayOrigin, rayDirection, rayTime);
}

void Camera::Resize(uint32_t width, uint32_t height)
{
    float aspectRatio = (float)width / (float)height;

    float theta = MathUtil::DegreeToRadians(m_FOV);
    float h = std::tan(theta / 2.0f);
    float viewportHeight = 2.0f * h * m_FocusDistance;
    float viewportWidth = aspectRatio * viewportHeight;

    glm::vec3 viewportU = viewportWidth * m_U;
    glm::vec3 viewportV = viewportHeight * -m_V;

    m_PixelDeltaU = viewportU / (float)width;
    m_PixelDeltaV = viewportV / (float)height;

    glm::vec3 viewportUpperLeft = m_Center - m_FocusDistance * m_W - viewportU / 2.0f - viewportV / 2.0f;
    m_Pixel00Location = viewportUpperLeft + 0.5f * (m_PixelDeltaU, m_PixelDeltaV);
}

void Camera::SetFocus(float defocusAngle, float focusDistance)
{
    m_DefocusAngle = defocusAngle;
    m_FocusDistance = focusDistance;

    float defocusRadius = focusDistance * std::tan(MathUtil::DegreeToRadians(defocusAngle / 2.0f));
    m_DefocusDiskU = m_U * defocusRadius;
    m_DefocusDiskV = m_V * defocusRadius;
}

void Camera::SetDirection(glm::vec3 lookFrom, glm::vec3 lookAt, glm::vec3 vUp, float vfov)
{
    m_Center = lookFrom;
    m_FOV = vfov;

    m_W = glm::normalize(lookFrom - lookAt);
    m_U = glm::normalize(glm::cross(vUp, m_W));
    m_V = glm::cross(m_W, m_U);
}

glm::vec3 Camera::DefocusDiskSample() const
{
    glm::vec2 p = Walnut::Random::InUnitDisk();
    return m_Center + p.x * m_DefocusDiskU + p.y * m_DefocusDiskV;
}
