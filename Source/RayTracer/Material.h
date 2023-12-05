#pragma once
#include "Random.h"
#include "Ray.h"
#include "Color.h"
#include <glm/glm.hpp>
class Material
{
public:
	virtual bool Scatter(const ray_t& ray, const raycastHit_t& raycastHit, color3_t& color, ray_t& scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const color3_t& albedo) : m_albedo{ albedo } {}
	bool Scatter(const ray_t& ray, const raycastHit_t& raycastHit, color3_t& color, ray_t& scattered) const override
	{
		glm::vec3 target = raycastHit.point + raycastHit.normal + randomInUnitSphere();
		glm::vec3 direction = glm::normalize(target - raycastHit.point);
		scattered = { raycastHit.point, direction };
		color = m_albedo;
		return true;
	}
	

protected:
	color3_t m_albedo;
};

class Metal : public Material
{
public:
	Metal(const glm::vec3& albedo, float fuzz) : m_albedo{ albedo }, m_fuzz{ fuzz } {}
	virtual bool Scatter(const ray_t& ray, const raycastHit_t& raycastHit, glm::vec3& color, ray_t& scattered) const override
	{
		glm::vec3 reflected = reflect(glm::normalize(ray.direction), raycastHit.normal);

		// set scattered ray from reflected ray + random point in sphere (fuzz = 0 no randomness, fuzz = 1 random reflected)
		// a mirror has a fuzz value of 0 and a diffused metal surface a higher value
		scattered = ray_t{ raycastHit.point, reflected + (randomInUnitSphere() * m_fuzz) };
		color = m_albedo;

		// make sure that reflected ray is going away from surface normal (dot product > 0, angle between vectors < 90 degrees)
		return glm::dot(scattered.direction, raycastHit.normal) > 0;
	}

protected:
	glm::vec3 m_albedo{ 0 };
	float m_fuzz = 0;
};
