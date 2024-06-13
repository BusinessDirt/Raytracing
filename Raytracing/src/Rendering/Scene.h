#pragma once

#include <string>
#include <vector>

#include "Objects/HittableList.h"
#include "Rendering/Camera.h"

struct Scene
{
	HittableList World;
	Camera Camera;
	std::string Name;
	glm::vec3 Background;
};

class SceneList
{
public:
	SceneList();

	void Add(Scene scene);

	void Setup(uint32_t width, uint32_t height);

	Scene* Get(int index);
	std::vector<const char*> GetSceneNames();

	void Clear() { m_Scenes.clear(); }
	size_t Size() { return m_Scenes.size(); }

private:
	std::vector<Scene> m_Scenes;
};

Scene GenerateRandomScene(uint32_t width, uint32_t height);
Scene GenerateCheckeredSpheres(uint32_t width, uint32_t height);
Scene GenerateEarthScene(uint32_t width, uint32_t height);
Scene GeneratePerlinSpheres(uint32_t width, uint32_t height);
Scene GenerateQuadScene(uint32_t width, uint32_t height);
Scene GenerateSimpleLightScene(uint32_t width, uint32_t height);
Scene GenerateCornellBoxScene(uint32_t width, uint32_t height);
Scene GenerateCornellSmokeScene(uint32_t width, uint32_t height);
Scene GenerateFinalScene(uint32_t width, uint32_t height);