#pragma once

#include "Objects/Hittable.h"
#include "Objects/Material.h"

#include "Rendering/Texture.h"

class ConstantMedium : public Hittable
{
public:
	ConstantMedium(Ref<Hittable> boundary, float density, Ref<Texture> texture);
	ConstantMedium(Ref<Hittable> boundary, float density, const glm::vec3& albedo);

	virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

	virtual AABB BoundingBox() const override { return m_Boundary->BoundingBox(); }

private:
	Ref<Hittable> m_Boundary;
	float m_NegativeInverseDensity;
	Ref<Material::Blank> m_PhaseFunction;
};