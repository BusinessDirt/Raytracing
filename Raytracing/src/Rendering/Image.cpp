#include "Rendering/Image.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stb_image.h>

#include <cstdlib>
#include <iostream>

#include "Math/MathUtil.h"

#define BYTES_PER_PIXEL 3

RTImage::RTImage(const std::string& filename)
{	
	if (Load("res/" + filename)) return;
	std::cerr << "ERROR: Could not load image file '" << filename.c_str() << "'.\n";
}

RTImage::~RTImage()
{
	delete[] m_ByteData;
	stbi_image_free(m_FloatData);
}

bool RTImage::Load(const std::string& filename)
{
	int n = BYTES_PER_PIXEL;
	m_FloatData = stbi_loadf(filename.c_str(), &m_Width, &m_Height, &n, BYTES_PER_PIXEL);
	if (m_FloatData == nullptr) return false;

	ConvertToBytes();
	return true;
}

const unsigned char* RTImage::Pixel(int x, int y) const
{
	static unsigned char magenta[] = { 255, 0, 255 };
	if (m_ByteData == nullptr) return magenta;

	x = MathUtil::Clamp(x, 0, m_Width);
	y = MathUtil::Clamp(y, 0, m_Height);

	return m_ByteData + y * m_Width * BYTES_PER_PIXEL + x * BYTES_PER_PIXEL;
}

unsigned char RTImage::FloatToByte(float value)
{
	if (value <= 0.0f) return 0;
	if (1.0f <= value) return 255;
	return static_cast<unsigned char>(256.0f * value);
}

void RTImage::ConvertToBytes()
{
	int totalBytes = m_Width * m_Height * BYTES_PER_PIXEL;
	m_ByteData = new unsigned char[totalBytes];

	unsigned char* byteDataPtr = m_ByteData;
	float* floatDataPtr = m_FloatData;
	for (int i = 0; i < totalBytes; i++, floatDataPtr++, byteDataPtr++)
		*byteDataPtr = FloatToByte(*floatDataPtr);
}

#ifdef _MSC_VER
	#pragma warning(pop)
#endif