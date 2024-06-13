#pragma once

#include "Math/AABB.h"
#include "Objects/Hittable.h"
#include "Rendering/Camera.h"

#include <memory>
#include <vector>

class HittableList : public Hittable 
{
public:
	HittableList() = default;
	HittableList(std::shared_ptr<Hittable> object) { Add(object); }

	void Clear() { m_Objects.clear(); }

	template<typename T, typename... Args>
	void Add(Args&&... args) 
	{
		static_assert(std::is_base_of<Hittable, T>::value, "Type must be derived from Hittable");
		Add(CreateRef<T>(std::forward<Args>(args)...));
	}

	void Add(Ref<Hittable> object);

	virtual bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

	std::vector<Ref<Hittable>> Objects() const { return m_Objects; }
	AABB BoundingBox() const override { return m_BoundingBox; }

private:
	std::vector<Ref<Hittable>> m_Objects;

	AABB m_BoundingBox;
};
