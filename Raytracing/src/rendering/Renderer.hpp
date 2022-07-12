#pragma once

#include "Walnut/Random.h"

#include <math.h>
#include <memory>
#include <ppl.h>
#include <future>
#include <algorithm>

#include "Camera.hpp"
#include "Util.hpp"
#include "rendering\Ray.hpp"
#include "objects\Sphere.hpp"
#include "objects\HittableList.hpp"
#include "ThreadPool.h"

using namespace std;
using namespace concurrency;
using namespace Raytracing::Utils;

class Renderer
{
public:
	enum class RenderState { Ready, Running, Finished, Stopped };

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
		std::fill_n(m_ImageData, width * height, static_cast<uint32_t>(0xff1e1e1e));

		m_FinalImage->SetData(m_ImageData);

		m_World.clear();
		m_World.add(make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5));
		m_World.add(make_shared<Sphere>(glm::vec3(0, -100.5, -1), 100));

		m_Camera.Resize(width, height);
	}

	void StartRender(int samples, int maxDepth) 
	{

		switch (m_State) {
			case RenderState::Finished:
			case RenderState::Stopped:
				m_RenderingThread.join();
				break;
			default:
				break;
		}

		m_RenderingThread = std::thread(&Renderer::Render, this, samples, maxDepth);
	}

	void StopRender() {
		m_State = RenderState::Stopped;
	}

	void Render(int samples, int maxDepth)
	{

		m_State = RenderState::Running;

		auto renderLine = [this](const unsigned int y, int samples, int maxDepth) {
			if (m_State == RenderState::Stopped) return;

			for (unsigned int x = 0; x < m_FinalImage->GetWidth(); ++x)
			{
				PerPixel(x, y, samples, maxDepth);
			}
			//m_FinalImage->SetData(m_ImageData);
		};

		std::vector<std::future<void>> futures;
		for (int j = m_FinalImage->GetHeight() - 1; j >= 0; --j) {
			 futures.push_back(m_ThreadPool.AddTask(renderLine, j, samples, maxDepth));
		}

		std::for_each(begin(futures), end(futures), [](auto& future) { future.wait(); });

		m_FinalImage->SetData(m_ImageData);

		m_State = RenderState::Finished;
	}

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

private:

	void PerPixel(int x, int y, int samples, int maxDepth)
	{
		glm::vec3 pixelColor(0, 0, 0);
		for (int s = 0; s < samples; s++)
		{
			float u = (x + RandomDouble()) / (m_FinalImage->GetWidth() - 1.0f);
			float v = (y + RandomDouble()) / (m_FinalImage->GetHeight() - 1.0f);
			Ray ray = m_Camera.getRay(u, v);
			pixelColor += RayColor(ray, maxDepth);
		}

		WritePixelToBuffer(m_ImageData, x, y, samples, pixelColor);
	}

	glm::vec3 RayColor(const Ray& r, int depth) 
	{
		HitRecord record;

		// If the ray bounce limit is exceeded, no more light is gathered
		if (depth <= 0)
			return glm::vec3(0, 0, 0);

		if (m_World.hit(r, 0, std::numeric_limits<double>::infinity(), record)) 
		{
			glm::vec3 target = record.p + record.normal + RandomPointInUnitSphere();
			return 0.5f * RayColor(Ray(record.p, target - record.p), depth - 1);
		}

		glm::vec3 unitDirection = glm::normalize(r.dir);
		float t = 0.5f * (unitDirection.y + 1.0f);
		return (1.0f - t) * glm::vec3(1, 1, 1) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}

	void WritePixelToBuffer(uint32_t *buffer, unsigned int x, unsigned int y, unsigned int samples, glm::vec3 color) const
	{
		color /= samples;
		color = glm::sqrt(color);
		color = glm::clamp(color, 0.0f, 1.0f);

		const unsigned int index = x + y * m_FinalImage->GetWidth();
		buffer[index] = 0xff000000 | (static_cast<uint8_t>(255 * color.z) << 16)
								   | (static_cast<uint8_t>(255 * color.g) << 8)
								   |  static_cast<uint8_t>(255 * color.r);
	}
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	
	HittableList m_World;
	Camera m_Camera;

	std::thread m_RenderingThread;
	Raytracing::Utils::Pool m_ThreadPool{};

	RenderState m_State = RenderState::Ready;
};
