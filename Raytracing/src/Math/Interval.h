#pragma once

#include <limits>

class Interval {
public:
	Interval() 
		: m_Min(std::numeric_limits<float>::infinity()), m_Max(-std::numeric_limits<float>::infinity())
	{}

	Interval(float min, float max)
		: m_Min(min), m_Max(max) 
	{}

	Interval(const Interval& a, const Interval& b);

	float Size() const { return m_Max - m_Min; }

	float min() const { return m_Min; }
	float max() const { return m_Max; }

	void min(float min) { m_Min = min; }
	void max(float max) { m_Max = max; }

	bool Contains(float x) const { return m_Min <= x && x <= m_Max; }
	bool Surrounds(float x) const { return m_Min < x && x < m_Max; }

	float Clamp(float x) const;
	Interval Expand(float delta) const;

	static const Interval Empty, Universe;

private:
	float m_Min, m_Max;
};