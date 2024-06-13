#include "rtpch.h"
#include "Math/Perlin.h"

#include "Math/MathUtil.h"
#include "Walnut/Random.h"

#define POINT_COUNT 256

Perlin::Perlin()
{
	m_RandVec = new glm::vec3[POINT_COUNT];
	for (int i = 0; i < POINT_COUNT; i++) {
		m_RandVec[i] = glm::normalize(Walnut::Random::Vec3());
	}

	m_PermutationX = PerlinGeneratePermutation();
	m_PermutationY = PerlinGeneratePermutation();
	m_PermutationZ = PerlinGeneratePermutation();
}

Perlin::~Perlin()
{
	delete[] m_RandVec;
	delete[] m_PermutationX;
	delete[] m_PermutationY;
	delete[] m_PermutationZ;
}

float Perlin::Noise(const glm::vec3& point) const
{
	float u = point.x - std::floor(point.x);
	float v = point.y - std::floor(point.y);
	float w = point.z - std::floor(point.z);

	int i = int(std::floor(point.x));
	int j = int(std::floor(point.y));
	int k = int(std::floor(point.z));
	glm::vec3 corners[2][2][2];

	for (int deltaI = 0; deltaI < 2; deltaI++)
	{
		for (int deltaJ = 0; deltaJ < 2; deltaJ++)
		{
			for (int deltaK = 0; deltaK < 2; deltaK++)
			{
				corners[deltaI][deltaJ][deltaK] = m_RandVec[
					m_PermutationX[(i + deltaI) & 255] ^
					m_PermutationY[(j + deltaJ) & 255] ^
					m_PermutationZ[(k + deltaK) & 255] 
				];
			}
		}
	}

	return MathUtil::PerlinInterpolation(corners, u, v, w);
}

float Perlin::Turbulence(const glm::vec3& point, int depth) const
{
	float accumulation = 0.0f;
	glm::vec3 tempPoint = point;
	float weight = 1.0f;

	for (int i = 0; i < depth; i++)
	{
		accumulation += weight * Noise(tempPoint);
		weight *= 0.5f;
		tempPoint *= 2.0f;
	}

	return std::fabs(accumulation);
}

int* Perlin::PerlinGeneratePermutation()
{
	int* permutation = new int[POINT_COUNT];
	for (int i = 0; i < POINT_COUNT; i++)
		permutation[i] = i;

	Permute(permutation, POINT_COUNT);
	return permutation;
}

void Perlin::Permute(int* permutation, int n)
{
	for (int i = n - 1; i > 0; i--) {
		int target = Walnut::Random::UInt(0, i);
		int tmp = permutation[i];
		permutation[i] = permutation[target];
		permutation[target] = tmp;
	}
}
