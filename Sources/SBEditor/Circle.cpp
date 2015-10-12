/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Circle.h"
#include "Polygon.h"

using namespace sb;

Circle Circle::unit = Circle(Vec2::zero, 0.5f);

sb::Polygon sb::Circle::toPolygon(size_t sides) const {
	std::vector<Vec2> pts;
	if (sides <= 4)
		sides = 4;

	float _step = (2.0f * SbPI) / (float)(sides);

	for (size_t i = 0; i < sides; ++i) {
		float t = _step * i;
		float x = m_center.x + m_radius * cosf(t);
		float y = m_center.y + m_radius * sinf(t);
		pts.push_back(Vec2(x, y));
	}

	return Polygon(pts);
}
