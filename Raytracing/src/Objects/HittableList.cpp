#include "rtpch.h"
#include "Objects/HittableList.h"

#include <Walnut/Random.h>

#include <memory>
#include <vector>

void HittableList::Add(Ref<Hittable> object)
{
	m_Objects.push_back(object); 
	m_BoundingBox = AABB(m_BoundingBox, object->BoundingBox());
}

bool HittableList::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const {
	HitRecord tempRecord;
	bool hitAnything = false;
	float closest = rayInterval.max();

	for (const std::shared_ptr<Hittable>& object : m_Objects)
	{
		if (object->Hit(ray, Interval(rayInterval.min(), closest), tempRecord))
		{
			hitAnything = true;
			closest = tempRecord.Intersection;
			record = tempRecord;
		}
	}

	return hitAnything;
}