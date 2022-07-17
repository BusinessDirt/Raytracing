#pragma once

#include "Walnut/Random.h"

#include <math.h>
#include <memory>
#include <ppl.h>
#include <future>
#include <windows.h>
#include <chrono>
#include <string>

#include "objects/Hittable.hpp"
#include "objects/Sphere.hpp"
#include "objects/HittableList.hpp"
#include "objects/Material.hpp"

#include "Camera.hpp"
#include "Util.hpp"
#include "Ray.hpp"

#include "ThreadPool.h"

#include "glm/glm.hpp"

using std::make_shared;
using glm::min;
using namespace std;
using namespace concurrency;
using namespace Raytracing::Utils;

class Renderer
{
public:
	struct Quad {
		Quad(glm::uvec2 min, glm::uvec2 max) : minCoo{ min }, maxCoo{ max } {};
		glm::uvec2 minCoo;
		glm::uvec2 maxCoo;
	};

	enum class RenderState { Ready, Running, Finished, Stopped };

	Renderer() = default;

	void StartRender(uint32_t width, uint32_t height, int samples, int maxDepth)
	{
		if (m_FinalImage)
		{
			// No resize necessary
			if (m_FinalImage->GetWidth() != width || m_FinalImage->GetHeight() != height)
				m_FinalImage->Resize(width, height);
		}
		else
		{
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}

		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];
		std::fill_n(m_ImageData, width * height, (uint32_t) 0xff1e1e1e);

		m_World.clear();
		m_World = GenerateRandomScene();

		glm::vec3 lf(13, 2, 3);
		glm::vec3 lt(0, 0, 0);
		glm::vec3 vup(0, 1, 0);
		float distToFocus = 10;
		float aperture = 0.1f;

		m_Camera = Camera(lf, lt, vup, width, height, 20, aperture, distToFocus);

		m_RenderingThread = std::thread(&Renderer::Render, this, samples, maxDepth);
		m_RenderingThread.detach();
	}

	void StopRender() 
	{
		m_State = RenderState::Stopped;
	}

	void Update()
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

	void Render(int maxSamples, int maxDepth)
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		m_State = RenderState::Running;

		auto renderLine = [this](const unsigned int y, int samples, int maxDepth) {
			for (unsigned int x = 0; x < m_FinalImage->GetWidth(); ++x)
			{
				if (m_State == RenderState::Stopped) return;
				PerPixel(x, y, samples, maxDepth);
			}
		};

		auto renderQuad = [this](const glm::uvec2 minCoord, const glm::uvec2 maxCoord, int samples, int maxDepth)
		{
			for (unsigned int j = maxCoord.y; j > minCoord.y; --j) {
				for (unsigned int i = minCoord.x; i < maxCoord.x; ++i) {
					if (m_State == RenderState::Stopped) return;
					PerPixel(i, j, samples, maxDepth);
				}
			}
		};

		std::vector<std::future<void>> futures;
		/*for (int j = m_FinalImage->GetHeight() - 1; j >= 0; --j) {
			 futures.push_back(m_ThreadPool.AddTask(renderLine, j, samples, maxDepth));
		}*/

		for (const auto& [minCoo, maxCoo] : SplitImage(100)) {
			futures.push_back(m_ThreadPool.AddTask(renderQuad, minCoo, maxCoo, maxSamples, maxDepth));
		}

		std::for_each(begin(futures), end(futures), [](auto& future) { future.wait(); });

		m_State = RenderState::Finished;
		m_FinalImage->SetData(m_ImageData);

		std::chrono::steady_clock::time_point end = chrono::steady_clock::now();

		auto seconds = chrono::duration_cast<std::chrono::seconds>(end - start).count();
		auto milliseconds = chrono::duration_cast<std::chrono::milliseconds>(end - start).count() - (1000 * seconds);
		m_RenderingTime = string(std::to_string(seconds) + "s " + std::to_string(milliseconds) + "ms");
		std::cout << "Rendering Took " << m_RenderingTime << std::endl;
	}

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	std::vector<Quad> SplitImage(unsigned int quadSize) const {
		glm::uvec2 imageSize((unsigned int) m_FinalImage->GetWidth() - 1, (unsigned int) m_FinalImage->GetHeight() - 1);

		std::vector<Quad> result;
		const unsigned int nX = static_cast<unsigned int>(std::ceil(imageSize.x / static_cast<float>(quadSize)));
		const unsigned int nY = static_cast<unsigned int>(std::ceil(imageSize.y / static_cast<float>(quadSize)));

		for (unsigned int j = nY; j > 0; --j) {
			for (unsigned int i = 0; i < nX; ++i) {
				glm::uvec2 minCoord(i * quadSize, (j - 1) * quadSize);
				glm::uvec2 maxCoord(
					min((i + 1) * quadSize, imageSize.x), 
					min(j * quadSize, imageSize.y)
				);
				result.emplace_back(minCoord, maxCoord);
			}
		}
		return result;
	}

	string GetRenderTime()
	{
		return m_RenderingTime;
	}

private:

	void PerPixel(int x, int y, int samples, int maxDepth)
	{
		glm::vec3 pixelColor(0, 0, 0);
		for (int s = 0; s < samples; s++)
		{
			float u = (x + RandomDouble()) / (m_FinalImage->GetWidth() - 1.0f);
			float v = (y + RandomDouble()) / (m_FinalImage->GetHeight() - 1.0f);
			Ray ray = m_Camera.GetRay(u, v);
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

		if (m_World.hit(r, 0.001, std::numeric_limits<double>::infinity(), record))
		{
			Ray scattered;
			glm::vec3 attenuation;
			if (record.materialPtr->Scatter(r, record, attenuation, scattered))
				return attenuation * RayColor(scattered, depth - 1);
			return glm::vec3(0, 0, 0);
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
	string m_RenderingTime = string("0s");
};
