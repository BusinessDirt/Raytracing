#include "rtpch.h"
#include "Math/Interval.h"

const Interval Interval::Empty		=	Interval( INFINITY, -INFINITY);
const Interval Interval::Universe	=	Interval(-INFINITY,  INFINITY);

Interval::Interval(const Interval& a, const Interval& b)
{
	// Create the interval tightly enclosing the two input intervals.
	m_Min = a.min() <= b.min() ? a.min() : b.min();
	m_Max = a.max() >= b.max() ? a.max() : b.max();
}

float Interval::Clamp(float x) const
{
	if (x < m_Min) return m_Min;
	if (x > m_Max) return m_Max;
	return x;
}

Interval Interval::Expand(float delta) const
{
	float padding = delta / 2.0f;
	return Interval(m_Min - padding, m_Max + padding);
}
