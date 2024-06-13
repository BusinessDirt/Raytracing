#pragma once

#include "Objects/Hittable.h"
#include "Objects/HittableList.h"

class Quad : public Hittable
{
public:
	Quad(const glm::vec3& startingCorner, const glm::vec3& u, const glm::vec3& v, Ref<Material::Blank> material);

	virtual void SetBoundingBox();
	virtual bool IsInterior(float a, float b, HitRecord& record) const;

	bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

	AABB BoundingBox() const override { return m_BoundingBox; }

private:
	glm::vec3 m_StartingCorner;
	glm::vec3 m_U, m_V, m_W;
	glm::vec3 m_Normal;
	float m_D;

	Ref<Material::Blank> m_Material;
	AABB m_BoundingBox;
};

Ref<HittableList> Box(const glm::vec3& a, const glm::vec3& b, Ref<Material::Blank> material);