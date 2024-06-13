#pragma once

#include "Objects/HittableList.h"
#include "Objects/Sphere.h"
#include "Objects/Material.h"

#include "Math/MathUtil.h"
#include "Math/BVH.h"

#include <Walnut/Random.h>

#include <memory>
#include <vector>

void HittableList::Add(std::shared_ptr<Hittable> object)
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

HittableList GenerateRandomScene() 
{
	HittableList world;

	std::shared_ptr<Texture> checker = std::make_shared<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));
	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, std::make_shared<Material::Lambertian>(checker)));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float chosenMaterial = Walnut::Random::Float(0.0f, 1.0f);
			glm::vec3 center(a + 0.9f * Walnut::Random::Float(-1.0f, 1.0f), 0.2f, b + 0.9f * Walnut::Random::Float(-1.0f, 1.0f));

			if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
			{
				if (chosenMaterial < 0.8f)
				{
					// diffuse
					glm::vec3 albedo = Walnut::Random::Vec3() * Walnut::Random::Vec3();
					std::shared_ptr<Material::Lambertian> sphereMaterial = std::make_shared<Material::Lambertian>(albedo);
					glm::vec3 center2 = center + glm::vec3(0.0f, Walnut::Random::Float(0.0f, 0.5f), 0.0f);
					world.Add(std::make_shared<Sphere>(center, center2, 0.2f, sphereMaterial));
				}
				else if (chosenMaterial < 0.95f)
				{
					// metal
					glm::vec3 albedo = Walnut::Random::Vec3(0.5f, 1.0f);
					float fuzz = Walnut::Random::Float(0.0f, 0.5f);
					std::shared_ptr<Material::Metal> sphereMaterial = std::make_shared<Material::Metal>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				else
				{
					// glass
					std::shared_ptr<Material::Dielectric> sphereMaterial = std::make_shared<Material::Dielectric>(1.5f);
					world.Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
			}
		}
	}

	std::shared_ptr<Material::Dielectric> material1 = std::make_shared<Material::Dielectric>(2.5f);
	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, material1));

	std::shared_ptr<Material::Lambertian> material2 = std::make_shared<Material::Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
	world.Add(std::make_shared<Sphere>(glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

	std::shared_ptr<Material::Metal> material3 = std::make_shared<Material::Metal>(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f);
	world.Add(std::make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

	world = HittableList(std::make_shared<BVHNode>(world));

	return world;
}

HittableList GenerateCheckeredSpheres()
{
	HittableList world;

	std::shared_ptr<Texture> checker = std::make_shared<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));

	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, -10.0f, 0.0f), 10.0f, std::make_shared<Material::Lambertian>(checker)));
	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f,  10.0f, 0.0f), 10.0f, std::make_shared<Material::Lambertian>(checker)));

	return world;
}

HittableList GenerateEarthScene()
{
	HittableList world;

	std::shared_ptr<ImageTexture> earthTexture = std::make_shared<ImageTexture>(std::string("earthmap.jpg"));
	std::shared_ptr<Material::Lambertian> earthSurface = std::make_shared<Material::Lambertian>(earthTexture);
	std::shared_ptr<Sphere> earth = std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, earthSurface);
	world.Add(earth);

	return world;
}

HittableList GeneratePerlinSpheres()
{
	HittableList world;

	std::shared_ptr<NoiseTexture> perlinTexture = std::make_shared<NoiseTexture>(4.0f);
	std::shared_ptr<Material::Lambertian> perlinMaterial = std::make_shared<Material::Lambertian>(perlinTexture);

	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, perlinMaterial));
	world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, 2.0f, 0.0f), 2.0f, perlinMaterial));

	return world;
}
