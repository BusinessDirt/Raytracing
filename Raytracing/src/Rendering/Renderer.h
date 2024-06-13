#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <string>
#include <thread>

#include "Objects/Hittable.h"
#include "Objects/Sphere.h"
#include "Objects/HittableList.h"

#include "Rendering/Camera.h"

#include "Math/Ray.h"

#include "glm/glm.hpp"


class Renderer
{
public:

	enum class RenderState { Ready, Running, Finished, Stopped };

	Renderer() = default;

	void StartRender(uint32_t width, uint32_t height, int samples, int maxDepth, int sceneIndex);
	void StopRender();

	void Update();

	void Render(int maxSamples, int maxDepth);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	std::string GetRenderTime() { return m_RenderingTime; }

private:

	void PerPixel(int x, int y, int samples, int maxDepth);

	void WritePixelToBuffer(uint32_t* buffer, unsigned int x, unsigned int y, unsigned int samples, glm::vec3 color) const;

	glm::vec3 RayColor(const Ray& r, int depth);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

	HittableList m_World;
	Camera m_Camera;

	std::thread m_RenderingThread;

	RenderState m_State = RenderState::Ready;
	std::string m_RenderingTime = std::string("0s");
};
#pragma once
