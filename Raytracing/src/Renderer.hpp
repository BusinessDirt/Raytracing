#include "Walnut/Random.h"

#include <math.h>

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
	}

	void Render()
	{
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
				coord = coord * 2.0f - 1.0f; // -1 -> 1
				coord.x *= (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
				m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
			}
		}

		m_FinalImage->SetData(m_ImageData);
	}

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	uint32_t PerPixel(glm::vec2 coord)
	{
		glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
		glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
		float radius = 0.75f;

		// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
		// where
		// a = ray origin
		// b = ray direction
		// r = radius
		// t = hit distance

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		// Quadratic forumula discriminant:
		// b^2 - 4ac

		float discriminant = b * b - 4.0f * a * c;
		float t = 0.0f;
		if (discriminant < 0.0f)
		{
			t = -1.0f;
		}
		else {
			t = (-b - sqrt(discriminant)) / (2.0f * a);
		}

		if (t > 0.0) {
			rayDirection = glm::normalize(rayDirection);
			glm::vec3 normale = glm::normalize((rayOrigin + rayDirection * t) - glm::vec3(0, 0, -0.5f));
			glm::vec3 color(normale.x + 1, normale.y + 1, normale.z + 1);
			color *= 0.5f;
			return 0xff000000 | (static_cast<uint8_t>(color.z * 255.0f) << 16) | (static_cast<uint8_t>(color.y * 255.0f) << 8) | (static_cast<uint8_t>(color.x * 255.0f));
		}

		return 0xff000000;
	}
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};
