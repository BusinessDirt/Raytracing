#include "rtpch.h"
#include "ConstantMedium.h"

#include "Objects/Material.h"

ConstantMedium::ConstantMedium(Ref<Hittable> boundary, float density, Ref<Texture> texture)
    : m_Boundary(boundary), m_NegativeInverseDensity(-1.0f / density), m_PhaseFunction(CreateRef<Material::Isotropic>(texture))
{}

ConstantMedium::ConstantMedium(Ref<Hittable> boundary, float density, const glm::vec3& albedo)
    : m_Boundary(boundary), m_NegativeInverseDensity(-1.0f / density), m_PhaseFunction(CreateRef<Material::Isotropic>(albedo))
{}

bool ConstantMedium::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
    const bool enableDebug = false;
    const bool debugging = enableDebug && Walnut::Random::Float() < 0.00001f;

    HitRecord record1, record2;

    if (!m_Boundary->Hit(ray, Interval::Universe, record1)) return false;
    if (!m_Boundary->Hit(ray, Interval(record1.Intersection + 0.0001f, INFINITY), record2)) return false;

    if (debugging) std::clog << "\ntMin=" << record1.Intersection << ", tMax=" << record2.Intersection << "\n";

    if (record1.Intersection < rayInterval.min()) record1.Intersection = rayInterval.min();
    if (record2.Intersection > rayInterval.max()) record2.Intersection = rayInterval.max();

    if (record1.Intersection >= record2.Intersection) return false;

    float rayLength = glm::length(ray.Direction());
    float distanceInsideBoundary = (record2.Intersection - record1.Intersection) * rayLength;
    float hitDistance = m_NegativeInverseDensity * std::log(Walnut::Random::Float());

    if (hitDistance > distanceInsideBoundary) return false;

    record.Intersection = record1.Intersection + hitDistance / rayLength;
    record.Point = ray.At(record.Intersection);

    if (debugging)
    {
        std::clog << "hitDistance = " << hitDistance << "\n"
            << "record.Intersection = " << record.Intersection << "\n"
            << "record.Point = vec3(" << record.Point.x << ", " << record.Point.y << ", " << record.Point.z << ")\n";
    }

    record.Normal = glm::vec3(1.0f, 0.0f, 0.0f);
    record.FrontFace = true;
    record.MaterialPtr = m_PhaseFunction;

    return true;
}
