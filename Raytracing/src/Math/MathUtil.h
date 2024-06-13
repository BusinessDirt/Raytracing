#pragma once

#include <glm/glm.hpp>
#include <Walnut/Random.h>

#define PI 3.1415926535897932385f

class MathUtil 
{
public:
    static glm::vec3 SampleSquare() 
    {
        return glm::vec3(Walnut::Random::Float() - 0.5f, Walnut::Random::Float() - 0.5f, 0.0f);
    }

    static glm::vec3 SampleDisk(float radius) 
    {
        glm::vec2 p = radius * Walnut::Random::InUnitDisk();
        return glm::vec3(p.x, p.y, 0.0f);
    }

    static float DegreeToRadians(float degrees) 
    {
        return degrees * PI / 180.0f;
    }

    static bool NearZero(glm::vec3& vec)
    {
        const double s = 1e-8;
        return (fabs(vec.x) < s) && (fabs(vec.y) < s) && (fabs(vec.z) < s);
    }

    static int Clamp(int x, int low, int high)
    {
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    static float Min(const glm::vec3& v)
    {
        return std::min(v.x, std::min(v.y, v.z));
    }

    static float Max(const glm::vec3& v)
    {
        return std::max(v.x, std::max(v.y, v.z));
    }

    static float TrilinearInterpolation(float corners[2][2][2], float u, float v, float w)
    {
        float accumulation = 0.0f;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    accumulation += (i * u + (1-i) * (1-u)) * (j * v + (1 - j) * (1 - v))
                        * (k * w + (1 - k) * (1 - w)) * corners[i][j][k];
                }
            }
        }

        return accumulation;
    }

    static float PerlinInterpolation(const glm::vec3 corners[2][2][2], float u, float v, float w)
    {
        float uu = u * u * (3 - 2 * u);
        float vv = v * v * (3 - 2 * v);
        float ww = w * w * (3 - 2 * w);
        float accumulation = 0.0f;

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++) 
                {
                    glm::vec3 weightV(u - i, v - j, w - k);
                    accumulation += (i * uu + (1 - i) * (1 - uu))
                        * (j * vv + (1 - j) * (1 - vv))
                        * (k * ww + (1 - k) * (1 - ww))
                        * glm::dot(corners[i][j][k], weightV);
                }
            }
        }

        return accumulation;
    }
};