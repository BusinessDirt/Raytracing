#pragma once

#include "Math/AABB.h"
#include "Objects/Hittable.h"

#include <memory>
#include <vector>

class HittableList : public Hittable 
{
public:
	HittableList() = default;
	HittableList(std::shared_ptr<Hittable> object) { Add(object); }

	void Clear() { m_Objects.clear(); }

	void Add(std::shared_ptr<Hittable> object);

	virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

	std::vector<std::shared_ptr<Hittable>> Objects() const { return m_Objects; }
	AABB BoundingBox() const override { return m_BoundingBox; }

private:
	std::vector<std::shared_ptr<Hittable>> m_Objects;

	AABB m_BoundingBox;
};

HittableList GenerateRandomScene();
HittableList GenerateCheckeredSpheres();
HittableList GenerateEarthScene();
HittableList GeneratePerlinSpheres();
