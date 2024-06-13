#include "Rendering/Texture.h"

#include "Math/Interval.h"

glm::vec3 CheckerTexture::Value(float u, float v, const glm::vec3& point) const
{
    int xInteger = int(std::floor(m_InverseScale * point.x));
    int yInteger = int(std::floor(m_InverseScale * point.y));
    int zInteger = int(std::floor(m_InverseScale * point.z));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? m_EvenTexture->Value(u, v, point) : m_OddTexture->Value(u, v, point);
}

glm::vec3 ImageTexture::Value(float u, float v, const glm::vec3& point) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (m_Image.GetHeight() <= 0) return glm::vec3(0.0f, 1.0f, 1.0f);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = Interval(0.0f, 1.0f).Clamp(u);
    v = 1.0 - Interval(0.0f, 1.0f).Clamp(v);  // Flip V to image coordinates

    int i = int(u * m_Image.GetWidth());
    int j = int(v * m_Image.GetHeight());
    const unsigned char* pixel = m_Image.Pixel(i, j);

    float colorScale = 1.0f / 255.0f;
    return glm::vec3(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
}
