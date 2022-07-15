#pragma once

#include "../rendering/Ray.hpp"
#include "glm/glm.hpp"
#include "../Util.hpp"
#include "Hittable.hpp"

namespace Material {
	class Blank {
	public:
		virtual bool Scatter(
			const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered
		) const = 0;
	};


	class Lambertian : public Blank {
	public:
		Lambertian(const glm::vec3& a) : albedo(a) {}

		virtual bool Scatter(
			const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered
		) const override {
			auto scatterDirection = record.normal + RandomUnitVector();

			if (NearZero(scatterDirection))
				scatterDirection = record.normal;

			scattered = Ray(record.p, scatterDirection);
			attenuation = albedo;
			return true;
		}

	public:
		glm::vec3 albedo;
	};


	class Metal : public Blank {
	public:
		Metal(const glm::vec3& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

		virtual bool Scatter(
			const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered
		) const override {
			glm::vec3 reflected = glm::reflect(glm::normalize(rayIn.dir), record.normal);
			scattered = Ray(record.p, reflected + fuzz * RandomPointInUnitSphere());
			attenuation = albedo;
			return (glm::dot(scattered.dir, record.normal) > 0);
		}

	public:
		glm::vec3 albedo;
		float fuzz;
	};


	class Dielectric : public Blank {
	public:
		Dielectric(float refractionIndex) : ri(refractionIndex), invRi(1.0f / refractionIndex) {}

		virtual bool Scatter(
			const Ray& rayIn, const HitRecord& record, glm::vec3& attenuation, Ray& scattered
		) const override {
			attenuation = glm::vec3(1.0, 1.0, 1.0);
			float refractionRatio = record.front_face ? invRi : ri;
			glm::vec3 unitDirection = glm::normalize(rayIn.direction());

			float cosTheta = std::fmin(glm::dot(-unitDirection, record.normal), 1.0f);
			float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

			bool cannotRefract = refractionRatio * sinTheta > 1.0f;
			glm::vec3 direction;

			if (cannotRefract || Reflectance(cosTheta, refractionRatio) > RandomDouble())
				direction = glm::reflect(unitDirection, record.normal);
			else
				direction = glm::refract(unitDirection, record.normal, refractionRatio);

			scattered = Ray(record.p, direction);
			return true;
		}

	private:
		static double Reflectance(double cosine, double ri)
		{
			auto r0 = (1 - ri) / (1 + ri);
			r0 = r0 * r0;
			return r0 + (1 - r0) * pow((1 - cosine), 5);
		}

	public:
		float ri, invRi;
	};
};

