#pragma once

#include "Hittable.hpp"
#include "Material.hpp"
#include "../rendering/Camera.hpp"
#include "Sphere.hpp"

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

HittableList GenerateRandomScene() {
	HittableList world;

	auto groundMaterial = make_shared<Material::Lambertian>(glm::vec3(0.5, 0.5, 0.5));
	world.add(make_shared<Sphere>(glm::vec3(0, -1000, 0), 1000, groundMaterial));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			double chosenMaterial = RandomDouble();
			glm::vec3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

			if (glm::length(center - glm::vec3(4, 0.2, 0)) > 0.9)
			{
				if (chosenMaterial < 0.8) 
				{
					// diffuse
					auto albedo = RandomVec3() * RandomVec3();
					shared_ptr<Material::Lambertian> sphereMaterial = make_shared<Material::Lambertian>(albedo);
					world.add(make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
				else if (chosenMaterial < 0.95)
				{
					// metal
					auto albedo = RandomVec3(0.5, 1);
					auto fuzz = RandomDouble(0, 0.5);
					shared_ptr<Material::Metal> sphereMaterial = make_shared<Material::Metal>(albedo, fuzz);
					world.add(make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
				else
				{
					// glass
					shared_ptr<Material::Dielectric> sphereMaterial = make_shared<Material::Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	auto material1 = make_shared<Material::Dielectric>(2.5);
	world.add(make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<Material::Lambertian>(glm::vec3(0.4, 0.2, 0.1));
	world.add(make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<Material::Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

	return world;
}
