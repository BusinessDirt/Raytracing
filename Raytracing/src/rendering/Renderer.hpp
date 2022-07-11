#pragma once

#include "Walnut/Random.h"

#include <math.h>
#include <memory>

#include "Camera.hpp"
#include "Util.hpp"
#include "rendering\Ray.hpp"
#include "objects\Sphere.hpp"
#include "objects\HittableList.hpp"

using std::make_shared;

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height)
	{
		if (m_FinalImage)
		{
			// No resize necessary
			if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
				return;

			m_FinalImage->Resize(width, height);
		}
		else
		{
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}

		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];

		m_World.clear();
		m_World.add(make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5));
		m_World.add(make_shared<Sphere>(glm::vec3(0, -100.5, -1), 100));

		m_Camera.Resize(width, height);
	}

	void Render(int samples)
	{
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(x, y, samples);
			}
		}

		m_FinalImage->SetData(m_ImageData);
	}

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	uint32_t PerPixel(int x, int y, int samples)
	{
		glm::vec3 pixelColor(0, 0, 0);
		for (int s = 0; s < samples; s++)
		{
			float u = (x + random_double()) / (m_FinalImage->GetWidth() - 1);
			float v = (y + random_double()) / (m_FinalImage->GetHeight() - 1);
			Ray ray = m_Camera.getRay(u, v);
			pixelColor += RayColor(ray);
		}

		
		return WriteColor(pixelColor, samples);
	}

	glm::vec3 RayColor(const Ray& r) 
	{
		HitRecord record;
		if (m_World.hit(r, 0, std::numeric_limits<double>::infinity(), record)) {
			return (record.normal + glm::vec3(1, 1, 1)) * 0.5f;
		}

		glm::vec3 unitDirection = glm::normalize(r.dir);
		float t = 0.5f * (unitDirection.y + 1.0f);
		return (1.0f - t) * glm::vec3(1, 1, 1) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}

	uint32_t WriteColor(glm::vec3 color, int samples)
	{
		float scale = 1.0f / samples;
		float r = color.x * scale;
		float g = color.y * scale;
		float b = color.z * scale;

		return 0xff000000 | (static_cast<uint8_t>(256 * clamp(b, 0.0, 0.999)) << 16) 
						  | (static_cast<uint8_t>(256 * clamp(g, 0.0, 0.999)) << 8)
						  | (static_cast<uint8_t>(256 * clamp(r, 0.0, 0.999)));
	}
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	
	HittableList m_World;
	Camera m_Camera;
};
