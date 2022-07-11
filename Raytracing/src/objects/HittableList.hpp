#pragma once

#include "Hittable.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable 
{
public:
	HittableList() {}
	HittableList(shared_ptr<Hittable> object) 
	{
		add(object);
	}

	void clear() 
	{
		objects.clear();
	}

	void add(shared_ptr<Hittable> object) 
	{
		objects.push_back(object);
	}

	virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override 
	{
		HitRecord tempRecord;
		bool hitAnything = false;
		auto closest = t_max;

		for (const auto& object : objects)
		{
			if (object->hit(r, t_min, closest, tempRecord))
			{
				hitAnything = true;
				closest = tempRecord.t;
				rec = tempRecord;
			}
		}

		return hitAnything;
	}
	

public:
	std::vector<shared_ptr<Hittable>> objects;
};
