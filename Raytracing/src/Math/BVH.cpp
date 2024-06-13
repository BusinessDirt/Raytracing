#include "BVH.h"

#include <Walnut/Random.h>

#include <algorithm>
#include <functional>

BVHNode::BVHNode(HittableList list)
	: BVHNode(list.Objects(), 0, list.Objects().size())
{
	// There's a C++ subtlety here. This constructor (without span indices) creates an
	// implicit copy of the hittable list, which we will modify. The lifetime of the copied
	// list only extends until this constructor exits. That's OK, because we only need to
	// persist the resulting bounding volume hierarchy.
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
{
	// Build the bounding box of the span of source objects.
	m_BoundingBox = AABB::Empty;
	for (size_t objectIndex = start; objectIndex < end; objectIndex++)
		m_BoundingBox = AABB(m_BoundingBox, objects[objectIndex]->BoundingBox());

	uint32_t axis = m_BoundingBox.LongestAxis();
	size_t objectSpan = end - start;

	bool(*comparator)(const std::shared_ptr<Hittable>&, const std::shared_ptr<Hittable>&) = 
		(axis == 0) ? BoxCompareX : (axis == 1) ? BoxCompareY : BoxCompareZ;

	if (objectSpan == 1) 
	{
		m_Left = m_Right = objects[start];
	}
	else if (objectSpan == 2) 
	{
		m_Left = objects[start];
		m_Right = objects[start + 1];
	}
	else 
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		size_t mid = start + objectSpan / 2;
		m_Left = std::make_shared<BVHNode>(objects, start, mid);
		m_Right = std::make_shared<BVHNode>(objects, mid, end);
	}
}

bool BVHNode::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
	if (!m_BoundingBox.Hit(ray, rayInterval))
		return false;

	bool hitLeft = m_Left->Hit(ray, rayInterval, record);
	bool hitRight = m_Right->Hit(ray, Interval(rayInterval.min(), hitLeft ? record.Intersection : rayInterval.max()), record);

	return hitLeft || hitRight;
}

bool BVHNode::BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int axisIndex)
{
	Interval aAxisInterval = a->BoundingBox().AxisInterval(axisIndex);
	Interval bAxisInterval = b->BoundingBox().AxisInterval(axisIndex);
	return aAxisInterval.min() < bAxisInterval.min();
}
