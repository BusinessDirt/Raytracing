#include "rtpch.h"
#include "Scene.h"

#include "Objects/Material.h"
#include "Objects/Sphere.h"
#include "Objects/ConstantMedium.h"
#include "Objects/Quad.h"

#include "Math/MathUtil.h"
#include "Math/BVH.h"

SceneList::SceneList()
	: m_Scenes()
{}

void SceneList::Add(Scene scene)
{
	m_Scenes.emplace_back(scene);
}

void SceneList::Setup(uint32_t width, uint32_t height)
{
	Add(GenerateRandomScene(width, height));
	Add(GenerateCheckeredSpheres(width, height));
	Add(GenerateEarthScene(width, height));
	Add(GeneratePerlinSpheres(width, height));
	Add(GenerateQuadScene(width, height));
	Add(GenerateSimpleLightScene(width, height));
	Add(GenerateCornellBoxScene(width, height));
	Add(GenerateCornellSmokeScene(width, height));
	Add(GenerateFinalScene(width, height));
}

Scene* SceneList::Get(int index)
{
	return &m_Scenes.at(index);
}

std::vector<const char*> SceneList::GetSceneNames()
{
	std::vector<const char*> names;
	for (const Scene& scene : m_Scenes)
		names.push_back(scene.Name.c_str());
	return names;
}


Scene GenerateRandomScene(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Floor
		Ref<Texture> checker = CreateRef<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));
		world.Add<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, CreateRef<Material::Lambertian>(checker));

		// Small Spheres
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
						Ref<Material::Lambertian> sphereMaterial = CreateRef<Material::Lambertian>(albedo);
						glm::vec3 center2 = center + glm::vec3(0.0f, Walnut::Random::Float(0.0f, 0.5f), 0.0f);
						world.Add<Sphere>(center, center2, 0.2f, sphereMaterial);
					}
					else if (chosenMaterial < 0.95f)
					{
						// metal
						glm::vec3 albedo = Walnut::Random::Vec3(0.5f, 1.0f);
						float fuzz = Walnut::Random::Float(0.0f, 0.5f);
						Ref<Material::Metal> sphereMaterial = CreateRef<Material::Metal>(albedo, fuzz);
						world.Add<Sphere>(center, 0.2f, sphereMaterial);
					}
					else
					{
						// glass
						Ref<Material::Dielectric> sphereMaterial = CreateRef<Material::Dielectric>(1.5f);
						world.Add<Sphere>(center, 0.2f, sphereMaterial);
					}
				}
			}
		}

		// Big spheres
		Ref<Material::Dielectric> material1 = CreateRef<Material::Dielectric>(2.5f);
		world.Add<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, material1);

		Ref<Material::Lambertian> material2 = CreateRef<Material::Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
		world.Add<Sphere>(glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2);

		Ref<Material::Metal> material3 = CreateRef<Material::Metal>(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f);
		world.Add<Sphere>(glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, material3);

		// BVH
		world = HittableList(CreateRef<BVHNode>(world));
	}

	// Camera
	Camera camera(glm::vec3(13.0f, 2.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 20.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Random Spheres"), glm::vec3(0.7f, 0.8f, 1.0f)};
}

Scene GenerateCheckeredSpheres(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Materials
		Ref<Texture> checker = CreateRef<CheckerTexture>(0.32f, glm::vec3(0.2f, 0.3f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));

		// Spheres
		world.Add<Sphere>(glm::vec3(0.0f, -10.0f, 0.0f), 10.0f, CreateRef<Material::Lambertian>(checker));
		world.Add<Sphere>(glm::vec3(0.0f, 10.0f, 0.0f), 10.0f, CreateRef<Material::Lambertian>(checker));
	}

	// Camera
	Camera camera(glm::vec3(13.0f, 2.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 20.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Checkered Spheres"), glm::vec3(0.7f, 0.8f, 1.0f) };
}

Scene GenerateEarthScene(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		Ref<ImageTexture> earthTexture = CreateRef<ImageTexture>(std::string("earthmap.jpg"));
		Ref<Material::Lambertian> earthSurface = CreateRef<Material::Lambertian>(earthTexture);
		world.Add<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, earthSurface);
	}

	// Camera
	Camera camera(glm::vec3(0.0f, 0.0f, 12.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 20.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Earth") };
}

Scene GeneratePerlinSpheres(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Materials
		Ref<NoiseTexture> perlinTexture = CreateRef<NoiseTexture>(4.0f);
		Ref<Material::Lambertian> perlinMaterial = CreateRef<Material::Lambertian>(perlinTexture);

		// Spheres
		world.Add<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, perlinMaterial);
		world.Add<Sphere>(glm::vec3(0.0f, 2.0f, 0.0f), 2.0f, perlinMaterial);
	}

	
	// Camera
	Camera camera(glm::vec3(13.0f, 2.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 20.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Perlin Spheres"), glm::vec3(0.7f, 0.8f, 1.0f) };
}

Scene GenerateQuadScene(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Materials
		Ref<Material::Lambertian> leftRed = CreateRef<Material::Lambertian>(glm::vec3(1.0f, 0.2f, 0.2f));
		Ref<Material::Lambertian> backGreen = CreateRef<Material::Lambertian>(glm::vec3(0.2f, 1.0f, 0.2f));
		Ref<Material::Lambertian> rightBlue = CreateRef<Material::Lambertian>(glm::vec3(0.2f, 0.2f, 1.0f));
		Ref<Material::Lambertian> upperOrange = CreateRef<Material::Lambertian>(glm::vec3(1.0f, 0.5f, 0.0f));
		Ref<Material::Lambertian> lowerTeal = CreateRef<Material::Lambertian>(glm::vec3(0.2f, 0.8f, 0.8f));

		// Quads
		world.Add<Quad>(glm::vec3(-3.0f, -2.0f, 5.0f), glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 4.0f, 0.0f), leftRed);
		world.Add<Quad>(glm::vec3(-2.0f, -2.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 4.0f, 0.0f), backGreen);
		world.Add<Quad>(glm::vec3(3.0f, -2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 4.0f, 0.0f), rightBlue);
		world.Add<Quad>(glm::vec3(-2.0f, 3.0f, 1.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 4.0f), upperOrange);
		world.Add<Quad>(glm::vec3(-2.0f, -3.0f, 5.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -4.0f), lowerTeal);
	}

	// Camera
	Camera camera(glm::vec3(0.0f, 0.0f, 9.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 80.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Quad Scene"), glm::vec3(0.7f, 0.8f, 1.0f) };
}

Scene GenerateSimpleLightScene(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Materials
		Ref<NoiseTexture> perlinTexture = CreateRef<NoiseTexture>(4.0f);
		Ref<Material::Lambertian> perlinMaterial = CreateRef<Material::Lambertian>(perlinTexture);
		Ref<Material::DiffuseLight> diffuseLight = CreateRef<Material::DiffuseLight>(glm::vec3(4.0f));

		// Spheres
		world.Add<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, perlinMaterial);
		world.Add<Sphere>(glm::vec3(0.0f, 2.0f, 0.0f), 2.0f, perlinMaterial);

		// Light
		world.Add<Quad>(glm::vec3(3.0f, 1.0f, -2.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f), diffuseLight);

	}


	// Camera
	Camera camera(glm::vec3(26.0f, 3.0f, 6.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 20.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Simple Light"), glm::vec3(0.0f) };
}

Scene GenerateCornellBoxScene(uint32_t width, uint32_t height) 
{
	// World
	HittableList world;
	{
		// Materials
		Ref<Material::Lambertian> red = CreateRef<Material::Lambertian>(glm::vec3(0.65f, 0.05f, 0.05f));
		Ref<Material::Lambertian> white = CreateRef<Material::Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
		Ref<Material::Lambertian> green = CreateRef<Material::Lambertian>(glm::vec3(0.12f, 0.45f, 0.15f));
		Ref<Material::DiffuseLight> light = CreateRef<Material::DiffuseLight>(glm::vec3(15.0f));

		// Quads
		world.Add<Quad>(glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), green);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), red);
		world.Add<Quad>(glm::vec3(343.0f, 554.0f, 332.0f), glm::vec3(-130.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -105.0f), light);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), white);
		world.Add<Quad>(glm::vec3(555.0f, 555.0f, 555.0f), glm::vec3(-555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -555.0f), white);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 555.0f), glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), white);

		// Boxes
		Ref<Hittable> box1 = Box(glm::vec3(0.0f), glm::vec3(165.0f, 330.0f, 165.0f), white);
		box1 = CreateRef<RotateY>(box1, 15.0f);
		box1 = CreateRef<Translate>(box1, glm::vec3(265.0f, 0.0f, 295.0f));
		world.Add(box1);

		Ref<Hittable> box2 = Box(glm::vec3(0.0f), glm::vec3(165.0f), white);
		box2 = CreateRef<RotateY>(box2, -18.0f);
		box2 = CreateRef<Translate>(box2, glm::vec3(130.0f, 0.0f, 65.0f));
		world.Add(box2);
	}


	// Camera
	Camera camera(glm::vec3(278.0f, 278.0f, -800.0f), glm::vec3(278.0f, 278.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 40.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Cornell Box"), glm::vec3(0.0f) };
}

Scene GenerateCornellSmokeScene(uint32_t width, uint32_t height)
{
	// World
	HittableList world;
	{
		// Materials
		Ref<Material::Lambertian> red = CreateRef<Material::Lambertian>(glm::vec3(0.65f, 0.05f, 0.05f));
		Ref<Material::Lambertian> white = CreateRef<Material::Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
		Ref<Material::Lambertian> green = CreateRef<Material::Lambertian>(glm::vec3(0.12f, 0.45f, 0.15f));
		Ref<Material::DiffuseLight> light = CreateRef<Material::DiffuseLight>(glm::vec3(15.0f));

		// Quads
		world.Add<Quad>(glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), green);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), red);
		world.Add<Quad>(glm::vec3(113.0f, 554.0f, 127.0f), glm::vec3(330.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 305.0f), light);
		world.Add<Quad>(glm::vec3(0.0f, 555.0f, 0.0f), glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), white);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 555.0f), white);
		world.Add<Quad>(glm::vec3(0.0f, 0.0f, 555.0f), glm::vec3(555.0f, 0.0f, 0.0f), glm::vec3(0.0f, 555.0f, 0.0f), white);

		// Boxes
		Ref<Hittable> box1 = Box(glm::vec3(0.0f), glm::vec3(165.0f, 330.0f, 165.0f), white);
		box1 = CreateRef<RotateY>(box1, 15.0f);
		box1 = CreateRef<Translate>(box1, glm::vec3(265.0f, 0.0f, 295.0f));
		world.Add<ConstantMedium>(box1, 0.01f, glm::vec3(0.0f));

		Ref<Hittable> box2 = Box(glm::vec3(0.0f), glm::vec3(165.0f), white);
		box2 = CreateRef<RotateY>(box2, -18.0f);
		box2 = CreateRef<Translate>(box2, glm::vec3(130.0f, 0.0f, 65.0f));
		world.Add<ConstantMedium>(box2, 0.01f, glm::vec3(1.0f));
	}

	// Camera
	Camera camera(glm::vec3(278.0f, 278.0f, -800.0f), glm::vec3(278.0f, 278.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 40.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Cornell Smoke"), glm::vec3(0.0f) };
}

Scene GenerateFinalScene(uint32_t width, uint32_t height)
{
	HittableList world;
	{
		HittableList boxes1;
		Ref<Material::Lambertian> ground = CreateRef<Material::Lambertian>(glm::vec3(0.48f, 0.83f, 0.53f));

		int boxesPerSide = 20;
		for (int i = 0; i < boxesPerSide; i++)
		{
			for (int j = 0; j < boxesPerSide; j++)
			{
				float w = 100.0f;
				float x0 = -1000.0f + i * w;
				float z0 = -1000.0f + j * w;
				float y0 = 0.0f;
				float x1 = x0 + w;
				float y1 = Walnut::Random::Float(1.0f, 101.0f);
				float z1 = z0 + w;

				boxes1.Add(Box(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1), ground));
			}
		}


		world.Add(CreateRef<BVHNode>(boxes1));

		Ref<Material::DiffuseLight> light = CreateRef<Material::DiffuseLight>(glm::vec3(7.0f));
		world.Add<Quad>(glm::vec3(123.0f, 554.0f, 147.0f), glm::vec3(300.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 265.0f), light);

		glm::vec3 center1 = glm::vec3(400.0f, 400.0f, 200.0f);
		glm::vec3 center2 = center1 + glm::vec3(30.0f, 0.0f, 0.0f);
		Ref<Material::Lambertian> sphere_material = CreateRef<Material::Lambertian>(glm::vec3(0.7f, 0.3f, 0.1f));
		world.Add<Sphere>(center1, center2, 50.0f, sphere_material);

		world.Add<Sphere>(glm::vec3(260.0f, 150.0f, 45.0f), 50.0f, CreateRef<Material::Dielectric>(1.5f));
		world.Add<Sphere>(glm::vec3(0.0f, 150.0f, 145.0f), 50.0f, CreateRef<Material::Metal>(glm::vec3(0.8f, 0.8f, 0.9f), 1.0f));

		Ref<Sphere> boundary = CreateRef<Sphere>(glm::vec3(360.0f, 150.0f, 145.0f), 70.0f, CreateRef<Material::Dielectric>(1.5f));
		world.Add(boundary);
		world.Add<ConstantMedium>(boundary, 0.2f, glm::vec3(0.2f, 0.4f, 0.9f));
		boundary = CreateRef<Sphere>(glm::vec3(0.0f), 5000.0f, CreateRef<Material::Dielectric>(1.5f));
		world.Add<ConstantMedium>(boundary, 0.0001f, glm::vec3(1.0f));

		Ref<Material::Lambertian> emat = CreateRef<Material::Lambertian>(CreateRef<ImageTexture>(std::string("earthmap.jpg")));
		world.Add<Sphere>(glm::vec3(400.0f, 200.0f, 400.0f), 100.0f, emat);
		Ref<NoiseTexture> pertext = CreateRef<NoiseTexture>(0.2f);
		world.Add<Sphere>(glm::vec3(220.0f, 280.0f, 300.0f), 80.0f, CreateRef<Material::Lambertian>(pertext));

		HittableList boxes2;
		Ref<Material::Lambertian> white = CreateRef<Material::Lambertian>(glm::vec3(0.73f));
		int ns = 1000;
		for (int j = 0; j < ns; j++)
			boxes2.Add<Sphere>(Walnut::Random::Vec3(0.0f, 165.0f), 10.0f, white);


		world.Add<Translate>(CreateRef<RotateY>(CreateRef<BVHNode>(boxes2), 15.0f), glm::vec3(-100.0f, 270.0f, 395.0f));
	}

	// Camera
	Camera camera(glm::vec3(478.0f, 278.0f, -600.0f), glm::vec3(278.0f, 278.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), width, height, 40.0f, 0.0f, 10.0f);

	return { world, camera, std::string("Final Scene"), glm::vec3(0.0f) };
}