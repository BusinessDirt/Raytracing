#include "rtpch.h"
#include <iostream>

#include "Rendering/Renderer.h"
#include "Objects/Material.h"
#include "Math/Interval.h"

void Renderer::StartRender(uint32_t width, uint32_t height, int samples, int maxDepth, Scene* scene)
{
	if (m_State == RenderState::Running) return;
	std::cout << "Started Render with " << width << "x" << height << "pixels, " << samples << " samples, " << maxDepth << " bounces\n";

	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() != width || m_FinalImage->GetHeight() != height)
			m_FinalImage->Resize(width, height);
	}
	else
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
	std::fill_n(m_ImageData, width * height, (uint32_t)0xff1e1e1e);

	// Resize camera
	scene->Camera.Resize(width, height);

	m_RenderingThread = std::thread(&Renderer::Render, this, samples, maxDepth, scene);
	m_RenderingThread.detach();
}

void Renderer::StopRender()
{
	m_State = RenderState::Stopped;
}

void Renderer::Update()
{
	if (m_State == RenderState::Running)
	{
		m_FinalImage->SetData(m_ImageData);
	}
	else if (m_State == RenderState::Finished || m_State == RenderState::Stopped)
	{
		if (m_RenderingThread.joinable())
		{
			m_RenderingThread.join();
		}
	}
}

void Renderer::Render(int maxSamples, int maxDepth, Scene* scene)
{
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	m_State = RenderState::Running;

	// Rendering
	for (uint32_t y = m_FinalImage->GetHeight() - 1; y > 0; y--) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			if (m_State == RenderState::Stopped) return;

			glm::vec3 pixelColor(0.0f);
			for (int s = 0; s < maxSamples; s++)
			{
				Ray ray = scene->Camera.GetRay(x, y);
				pixelColor += RayColor(ray, maxDepth, scene);
			}

			WritePixelToBuffer(m_ImageData, x, y, maxSamples, pixelColor);
		}
	}

	m_State = RenderState::Finished;
	m_FinalImage->SetData(m_ImageData);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() - (1000 * seconds);
	m_RenderingTime = std::string(std::to_string(seconds) + "s " + std::to_string(milliseconds) + "ms");
	std::cout << "Rendering Took " << m_RenderingTime << std::endl;
}

glm::vec3 Renderer::RayColor(const Ray& ray, int depth, Scene* scene)
{
	// If the ray bounce limit is exceeded, no more light is gathered
	if (depth <= 0) return glm::vec3(0.0f);

	HitRecord record;

	if (!scene->World.Hit(ray, Interval(0.001f, std::numeric_limits<float>::infinity()), record))
		return scene->Background;

	Ray scattered;
	glm::vec3 attenuation;
	glm::vec3 colorFromEmission = record.MaterialPtr->Emitted(record.U, record.V, record.Point);

	if (!record.MaterialPtr->Scatter(ray, record, attenuation, scattered))
		return colorFromEmission;

	glm::vec3 colorFromScatter = attenuation * RayColor(scattered, depth - 1, scene);
	return colorFromEmission + colorFromScatter;
}

void Renderer::WritePixelToBuffer(uint32_t* buffer, unsigned int x, unsigned int y, unsigned int samples, glm::vec3 color) const
{
	color /= samples;
	color = glm::sqrt(color);
	color = glm::clamp(color, 0.0f, 1.0f);

	const unsigned int index = x + y * m_FinalImage->GetWidth();
	buffer[index] = 0xff000000 | (static_cast<uint8_t>(255 * color.z) << 16)
		| (static_cast<uint8_t>(255 * color.g) << 8)
		| static_cast<uint8_t>(255 * color.r);
}
