#pragma once

#include <random>
#include "glm/glm.hpp"

inline double RandomDouble() 
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double RandomDouble(double min, double max) 
{
    // Returns a random real in [min,max).
    return min + (max - min) * RandomDouble();
}

//inline float RandomFloat()
//{
//    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
//    static std::mt19937 generator;
//    return distribution(generator);
//}
//
//inline float RandomFloat(float min, float max)
//{
//    return min + (max - min) * RandomFloat();
//}

inline static glm::vec3 RandomVec3()
{
    return glm::vec3(RandomDouble(), RandomDouble(), RandomDouble());
}

inline static glm::vec3 RandomVec3(double min, double max)
{
    return glm::vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
}

glm::vec3 RandomPointInUnitSphere()
{
    while (true)
    {
        glm::vec3 p = RandomVec3(-1, 1);
        if (p.x * p.x + p.y * p.y + p.z * p.z >= 1) continue;
        return p;
    }
}

inline double Clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}