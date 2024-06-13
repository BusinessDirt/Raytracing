#pragma once

#include <glm/glm.hpp>

class Perlin {
public:
	Perlin();
	~Perlin();
	float Noise(const glm::vec3& point) const;

private:
	static int* PerlinGeneratePermutation();
	static void Permute(int* perm, int n);

private:
	float* m_RandFloat;
	int* m_PermutationX;
	int* m_PermutationY;
	int* m_PermutationZ;
};