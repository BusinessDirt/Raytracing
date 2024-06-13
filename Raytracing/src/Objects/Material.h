#pragma once

#include "glm/glm.hpp"

#include "Rendering/Texture.h"

#include "Math/MathUtil.h"
#include "Math/Ray.h"

#include "Objects/Hittable.h"

namespace Material {

	class Blank 
	{
	public:
		virtual ~Blank() = default;

		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const = 0;

		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const = 0;
	};


	class Lambertian : public Blank 
	{
	public:
		Lambertian(const glm::vec3& albedo) 
			: m_Texture(CreateRef<SolidColor>(albedo)) 
		{}

		Lambertian(Ref<Texture> texture)
			: m_Texture(texture)
		{}

		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override;
		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const override { return glm::vec3(0.0f); }

	public:
		Ref<Texture> m_Texture;
	};


	class Metal : public Blank 
	{
	public:
		Metal(const glm::vec3& a, float f) 
			: m_Albedo(a), m_Fuzz(f < 1 ? f : 1) 
		{}

		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override;
		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const override { return glm::vec3(0.0f); }

	public:
		glm::vec3 m_Albedo;
		float m_Fuzz;
	};


	class Dielectric : public Blank 
	{
	public:
		Dielectric(float refractionIndex) 
			: m_RefractionIndex(refractionIndex), m_InverseRefractionIndex(1.0f / refractionIndex)
		{}

		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override;
		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const override { return glm::vec3(0.0f); }

	private:
		static double Reflectance(float cosine, float ri) {
			float r0 = (1 - ri) / (1 + ri);
			r0 = r0 * r0;
			return r0 + (1 - r0) * std::pow((1 - cosine), 5);
		}

	public:
		float m_RefractionIndex, m_InverseRefractionIndex;
	};

	class DiffuseLight : public Blank 
	{
	public:
		DiffuseLight(const glm::vec3& emitted)
			: m_Texture(CreateRef<SolidColor>(emitted))
		{}

		DiffuseLight(Ref<Texture> texture)
			: m_Texture(texture)
		{}
		
		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override { return 0; }
		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const override;

	private:
		Ref<Texture> m_Texture;
	};

	class Isotropic : public Blank
	{
	public:
		Isotropic(const glm::vec3& albedo) 
			: m_Texture(CreateRef<SolidColor>(albedo)) 
		{}

		Isotropic(Ref<Texture> texture)
			: m_Texture(texture)
		{}

		virtual bool Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override;
		virtual glm::vec3 Emitted(float u, float v, const glm::vec3& point) const override { return glm::vec3(0.0f); }

	private:
		Ref<Texture> m_Texture;
	};
};

