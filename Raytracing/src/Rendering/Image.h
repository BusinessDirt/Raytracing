#pragma once

#include <string>

class RTImage {
public:
	RTImage() = default;
	RTImage(const std::string& filename);

	~RTImage();

	bool Load(const std::string& filename);

	int GetWidth() const { return m_FloatData == nullptr ? 0 : m_Width; }
	int GetHeight() const { return m_FloatData == nullptr ? 0 : m_Height; }

	const unsigned char* Pixel(int x, int y) const;

private:
	static unsigned char FloatToByte(float value);

	void ConvertToBytes();

private:
	float* m_FloatData = nullptr;
	unsigned char* m_ByteData = nullptr;
	int m_Width, m_Height;
};