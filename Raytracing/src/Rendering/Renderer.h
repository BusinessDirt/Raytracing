#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <string>
#include <thread>

#include "Objects/Hittable.h"
#include "Objects/Sphere.h"
#include "Objects/HittableList.h"

#include "Rendering/Camera.h"
#include "Rendering/Scene.h"

#include "Math/Ray.h"

#include "glm/glm.hpp"


class Renderer
{
public:

	enum class RenderState { Ready, Running, Finished, Stopped };

	Renderer() = default;

	void StartRender(uint32_t width, uint32_t height, int samples, int maxDepth, Scene* scene);
	void StopRender();

	void Update();

	void Render(int maxSamples, int maxDepth, Scene* scene);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	std::string GetRenderTime() { return m_RenderingTime; }

private:

	void WritePixelToBuffer(uint32_t* buffer, unsigned int x, unsigned int y, unsigned int samples, glm::vec3 color) const;

	glm::vec3 RayColor(const Ray& r, int depth, Scene* scene);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

	std::thread m_RenderingThread;

	RenderState m_State = RenderState::Ready;
	std::string m_RenderingTime = std::string("0s");
};
#pragma once
