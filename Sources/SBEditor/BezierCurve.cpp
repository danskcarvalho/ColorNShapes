/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "BezierCurve.h"
#include "Polygon.h"

using namespace sb;

void renderCurve(const BezierCurve& b, float epsilon, std::vector<Vec2>& points) {
	if (b.isStraight(epsilon)) {
		points.push_back(b.point0);
		points.push_back(b.point3);
	}
	else {
		BezierCurve b1, b2;
		b.subdivide(&b1, &b2);
		renderCurve(b1, epsilon, points);
		points.pop_back(); //doesn't need the last one because b2 will have it.
		renderCurve(b2, epsilon, points);
	}
}

sb::Polygon sb::BezierCurve::toPolygon(size_t quality /*= 1000*/) const {
	assert(quality > 0);
	std::vector<Vec2> points;
	float epsilon = 1.0f / quality;
	renderCurve(*this, epsilon, points);
	if (point0 == point3 && points.size() > 2) //closed?
		points.pop_back(); //remove last one!

	return Polygon(std::move(points), point0 == point3);
}

sb::Polygon sb::BezierCurve::toPolygon(const std::vector<BezierCurve>& curves, size_t quality /*= 1000*/) {
	assert(quality > 0);
	assert(curves.size() >= 1);
	std::vector<Vec2> points;
	float epsilon = 1.0f / quality;
	size_t index = 0;
	Option<Vec2> lastEndPoint;
	for (const auto& bc : curves) {
		if (lastEndPoint.hasValue())
			assert(lastEndPoint.value() == bc.point0);
		renderCurve(bc, epsilon, points);
		if (index != (curves.size() - 1))
			points.pop_back();
		lastEndPoint = bc.point3;
		index++;
	}

	if (points.front() == points.back() && points.size() > 2) { //closed?
		points.pop_back(); //remove last one!
		return Polygon(std::move(points), true);
	}
	else
		return Polygon(std::move(points), false);
}

sb::Polygon sb::BezierCurve::toPolygon(const BezierCurve* curves, size_t count, size_t quality /*= 1000*/) {
	assert(quality > 0);
	assert(count >= 1);
	std::vector<Vec2> points;
	float epsilon = 1.0f / quality;
	Option<Vec2> lastEndPoint;
	for (size_t i = 0; i < count; i++) {
		if (lastEndPoint.hasValue())
			assert(lastEndPoint.value() == curves[i].point0);
		renderCurve(curves[i], epsilon, points);
		if (i != (count - 1))
			points.pop_back();
		lastEndPoint = curves[i].point3;
	}

	if (points.front() == points.back() && points.size() > 2) { //closed?
		points.pop_back(); //remove last one!
		return Polygon(std::move(points), true);
	}
	else
		return Polygon(std::move(points), false);
}

