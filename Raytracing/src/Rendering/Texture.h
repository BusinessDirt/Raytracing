#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Rendering/Image.h"

#include "Math/Perlin.h"

class Texture 
{
public:
	virtual ~Texture() = default;
	
	virtual glm::vec3 Value(float u, float v, const glm::vec3& point) const = 0;
};

class SolidColor : public Texture 
{
public:
	SolidColor(const glm::vec3& albedo) 
		: m_Albedo(albedo) 
	{}

	SolidColor(float red, float green, float blue) 
		: SolidColor(glm::vec3(red, green, blue)) 
	{}

	glm::vec3 Value(float u, float v, const glm::vec3& point) const override { return m_Albedo; }

private:
	glm::vec3 m_Albedo;
};

class CheckerTexture : public Texture 
{
public:
	CheckerTexture(float scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
		: m_InverseScale(1.0f / scale), m_EvenTexture(even), m_OddTexture(odd) 
	{}

	CheckerTexture(float scale, const glm::vec3& color1, const glm::vec3& color2)
		: m_InverseScale(1.0f / scale), m_EvenTexture(std::make_shared<SolidColor>(color1)), m_OddTexture(std::make_shared<SolidColor>(color2))
	{}

	glm::vec3 Value(float u, float v, const glm::vec3& point) const override;
private:
	float m_InverseScale;
	std::shared_ptr<Texture> m_EvenTexture, m_OddTexture;
};

class ImageTexture : public Texture 
{
public:
	ImageTexture(std::string& path) 
		: m_Image(path) 
	{}

	glm::vec3 Value(float u, float v, const glm::vec3& point) const override;

private:
	RTImage m_Image;
};

class NoiseTexture : public Texture 
{
public:
	NoiseTexture() = default;
	NoiseTexture(float scale) 
		: m_Scale(scale) 
	{}

	glm::vec3 Value(float u, float v, const glm::vec3& point) const override;
	
private:
	Perlin m_Noise;
	float m_Scale;
};