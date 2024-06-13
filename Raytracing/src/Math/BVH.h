#pragma once

#include "Math/AABB.h"
#include "Objects/Hittable.h"
#include "Objects/HittableList.h"

class BVHNode : public Hittable {
public:
	BVHNode(HittableList list);
	BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

	bool Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const override;

	AABB BoundingBox() const override { return m_BoundingBox; }

private:
	static bool BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int axisIndex);
	static bool BoxCompareX(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) { return BoxCompare(a, b, 0); }
	static bool BoxCompareY(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) { return BoxCompare(a, b, 1); }
	static bool BoxCompareZ(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) { return BoxCompare(a, b, 2); }

private:
	std::shared_ptr<Hittable> m_Left;
	std::shared_ptr<Hittable> m_Right;
	AABB m_BoundingBox;
};