#include "rtpch.h"
#include "Objects/Material.h"
#include <Walnut/Random.h>

namespace Material {
	
	bool Lambertian::Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const 
	{
		glm::vec3 scatterDirection = record.Normal + glm::normalize(Walnut::Random::InUnitSphere());

		if (MathUtil::NearZero(scatterDirection))
			scatterDirection = record.Normal;

		scattered = Ray(record.Point, scatterDirection, rayIn.time());
		attenuation = m_Texture->Value(record.U, record.V, record.Point);
		return true;
	}


	bool Metal::Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const 
	{
		glm::vec3 reflected = glm::reflect(rayIn.Direction(), record.Normal);
		scattered = Ray(record.Point, glm::normalize(reflected) + m_Fuzz * Walnut::Random::InUnitSphere(), rayIn.time());
		attenuation = m_Albedo;
		return glm::dot(scattered.Direction(), record.Normal) > 0;
	}

	bool Dielectric::Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const 
	{
		attenuation = glm::vec3(1.0, 1.0, 1.0);
		float refractionRatio = record.FrontFace ? m_InverseRefractionIndex : m_RefractionIndex;

		glm::vec3 unitDirection = glm::normalize(rayIn.Direction());
		float cosTheta = std::fmin(glm::dot(-unitDirection, record.Normal), 1.0f);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = refractionRatio * sinTheta > 1.0f;
		glm::vec3 direction;

		if (cannotRefract || Reflectance(cosTheta, refractionRatio) > Walnut::Random::Float())
			direction = glm::reflect(unitDirection, record.Normal);
		else
			direction = glm::refract(unitDirection, record.Normal, refractionRatio);

		scattered = Ray(record.Point, direction, rayIn.time());
		return true;
	}
	
	glm::vec3 DiffuseLight::Emitted(float u, float v, const glm::vec3& point) const
	{
		return m_Texture->Value(u, v, point);
	}

	bool Isotropic::Scatter(const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const
	{
		scattered = Ray(record.Point, glm::normalize(Walnut::Random::InUnitSphere()), rayIn.time());
		attenuation = m_Texture->Value(record.U, record.V, record.Point);
		return true;
	}
};

