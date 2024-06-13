#pragma once

#include <iostream>

#include "Rendering/Renderer.h"
#include "Objects/Material.h"
#include "Math/Interval.h"

void Renderer::StartRender(uint32_t width, uint32_t height, int samples, int maxDepth, int sceneIndex)
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

	m_World.Clear();
	switch (sceneIndex)
	{
		case 0: m_World = GenerateRandomScene(); break;
		case 1: m_World = GenerateCheckeredSpheres(); break;
		case 2: m_World = GenerateEarthScene(); break;
		case 3: m_World = GeneratePerlinSpheres(); break;
	}

	glm::vec3 lf(13.0f, 2.0f, 3.0f);
	glm::vec3 lt(0.0f);
	glm::vec3 vup(0.0f, 1.0f, 0.0f);
	float distToFocus = 10.0f;
	float aperture = 0.0f;

	m_Camera = Camera(lf, lt, vup, width, height, 20, aperture, distToFocus);

	m_RenderingThread = std::thread(&Renderer::Render, this, samples, maxDepth);
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

void Renderer::Render(int maxSamples, int maxDepth)
{
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	m_State = RenderState::Running;

	// Rendering
	for (uint32_t y = m_FinalImage->GetHeight() - 1; y > 0; y--) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			if (m_State == RenderState::Stopped) return;
			PerPixel(x, y, maxSamples, maxDepth);
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

void Renderer::PerPixel(int x, int y, int samples, int maxDepth)
{
	glm::vec3 pixelColor(0, 0, 0);
	for (int s = 0; s < samples; s++)
	{
		Ray ray = m_Camera.GetRay((float)x, (float)y);
		pixelColor += RayColor(ray, maxDepth);
	}

	WritePixelToBuffer(m_ImageData, x, y, samples, pixelColor);
}

glm::vec3 Renderer::RayColor(const Ray& ray, int depth)
{
	HitRecord record;

	// If the ray bounce limit is exceeded, no more light is gathered
	if (depth <= 0) return glm::vec3(0, 0, 0);

	if (m_World.Hit(ray, Interval(0.001f, std::numeric_limits<float>::infinity()), record))
	{
		Ray scattered;
		glm::vec3 attenuation;
		if (record.MaterialPtr->Scatter(ray, record, attenuation, scattered))
			return attenuation * RayColor(scattered, depth - 1);
		return glm::vec3(0.0f);
	}

	glm::vec3 unitDirection = glm::normalize(ray.Direction());
	float t = 0.5f * (unitDirection.y + 1.0f);
	return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
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
