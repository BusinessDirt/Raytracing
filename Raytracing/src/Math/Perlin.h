#pragma once

#include <glm/glm.hpp>

class Perlin {
public:
	Perlin();
	~Perlin();

	float Noise(const glm::vec3& point) const;
	float Turbulence(const glm::vec3& point, int depth) const;

private:
	static int* PerlinGeneratePermutation();
	static void Permute(int* perm, int n);

private:
	glm::vec3* m_RandVec;
	int* m_PermutationX;
	int* m_PermutationY;
	int* m_PermutationZ;
};