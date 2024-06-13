#include "rtpch.h"
#include "Quad.h"

Quad::Quad(const glm::vec3& startingCorner, const glm::vec3& u, const glm::vec3& v, Ref<Material::Blank> material)
	: m_StartingCorner(startingCorner), m_U(u), m_V(v), m_Material(material)
{
	glm::vec3 normal = glm::cross(m_U, m_V);
	m_Normal = glm::normalize(normal);
	m_D = glm::dot(m_Normal, m_StartingCorner);
	m_W = normal / glm::dot(normal, normal);

	SetBoundingBox();
}

void Quad::SetBoundingBox()
{
	AABB boundingBoxDiagonal1 = AABB(m_StartingCorner, m_StartingCorner + m_U + m_V);
	AABB boundingBoxDiagonal2 = AABB(m_StartingCorner + m_U, m_StartingCorner + m_V);
	m_BoundingBox = AABB(boundingBoxDiagonal1, boundingBoxDiagonal2);
}

bool Quad::IsInterior(float a, float b, HitRecord& record) const
{
	Interval unitInterval = Interval(0.0f, 1.0f);
	// Given the hit point in plane coordinates, return false if it is outside the
	// primitive, otherwise set the hit record UV coordinates and return true.

	if (!(unitInterval.Contains(a) && unitInterval.Contains(b))) return false;

	record.U = a;
	record.V = b;
	return true;
}

bool Quad::Hit(const Ray& ray, Interval rayInterval, HitRecord& record) const
{
	float denominator = glm::dot(m_Normal, ray.Direction());

	// No hit if the ray is parallel to the plane.
	if (std::fabs(denominator) < 1e-8f) return false;

	// Return false if the hit point parameter t is outside the ray interval
	float t = (m_D - glm::dot(m_Normal, ray.Origin())) / denominator;
	if (!rayInterval.Contains(t)) return false;

	// Determine if the hit point lies eithin the planar shape using its plane coordinates.
	glm::vec3 intersectionPoint = ray.At(t);
	glm::vec3 planarHitpointVector = intersectionPoint - m_StartingCorner;
	float alpha = glm::dot(m_W, glm::cross(planarHitpointVector, m_V));
	float beta = glm::dot(m_W, glm::cross(m_U, planarHitpointVector));

	if (!IsInterior(alpha, beta, record)) return false;

	// Ray hits the 2D shape; set the rest of the hir record and return true;
	record.Intersection = t;
	record.Point = intersectionPoint;
	record.MaterialPtr = m_Material;
	record.SetFaceNormal(ray, m_Normal);

	return true;
}

Ref<HittableList> Box(const glm::vec3& a, const glm::vec3& b, Ref<Material::Blank> material)
{
	// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
	Ref<HittableList> sides = CreateRef<HittableList>();

	// Construct the two opposite vertices with the minimum and maximum coordinates.
	glm::vec3 min = glm::vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
	glm::vec3 max = glm::vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

	glm::vec3 dx = glm::vec3(max.x - min.x, 0.0f, 0.0f);
	glm::vec3 dy = glm::vec3(0.0f, max.y - min.y, 0.0f);
	glm::vec3 dz = glm::vec3(0.0f, 0.0f, max.z - min.z);

	sides->Add<Quad>(glm::vec3(min.x, min.y, max.z), dx, dy, material); // front
	sides->Add<Quad>(glm::vec3(max.x, min.y, max.z), -dz, dy, material); // right
	sides->Add<Quad>(glm::vec3(max.x, min.y, min.z), -dx, dy, material); // back
	sides->Add<Quad>(glm::vec3(min.x, min.y, min.z), dz, dy, material); // left
	sides->Add<Quad>(glm::vec3(min.x, max.y, max.z), dx, -dz, material); // top
	sides->Add<Quad>(glm::vec3(min.x, min.y, min.z), dx, dz, material); // bottom

	return sides;
}
