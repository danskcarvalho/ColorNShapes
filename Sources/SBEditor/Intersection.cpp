/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "Intersection.h"
#include "Vec2.h"
#include "Ray.h"
#include "Line.h"
#include "LineSegment.h"
#include "Option.h"
#include "Utils.h"
#include "Rect.h"
#include "Circle.h"
#include "Polygon.h"

using namespace sb;

Option<Vec2>Intersection::get(const Ray& a, const Ray& b) {
	const auto dt = a.m_direction.x * b.m_direction.y - b.m_direction.x * a.m_direction.y;
	if (aeq(dt, 0.0f))
		return nullptr;
	const auto dx = a.m_point.x - b.m_point.x;
	const auto dy = a.m_point.y - b.m_point.y;
	auto u = (a.m_direction.x * dy - a.m_direction.y * dx) / dt;
	auto t = (b.m_direction.x * dy - b.m_direction.y * dx) / dt;

	auto pt = a.sampleAlongRay(t);
	if (aeq(pt, a.m_point)) {
		pt = a.m_point;
		t = 0;
	}
	if (aeq(pt, b.m_point)) {
		pt = b.m_point;
		u = 0;
	}
	assert(aeq(pt, b.sampleAlongRay(u)));
	return u < 0 || t < 0 ? nullptr : (Option<Vec2>)pt;
}

Option<Vec2> Intersection::get(const Line& a, const Line& b) {
	const auto dt = a.m_direction.x * b.m_direction.y - b.m_direction.x * a.m_direction.y;
	if (aeq(dt, 0.0f))
		return nullptr;
	const auto dx = a.m_pointA.x - b.m_pointA.x;
	const auto dy = a.m_pointA.y - b.m_pointA.y;
	const auto u = (a.m_direction.x * dy - a.m_direction.y * dx) / dt;
	const auto t = (b.m_direction.x * dy - b.m_direction.y * dx) / dt;

	auto pt = a.m_pointA + a.m_direction * t;
	assert(aeq(pt, b.m_pointA + b.m_direction * u));
	return pt;
}

bool Intersection::test(const Ray& a, const Vec2& b) {
	return a.containsPoint(b, true);
}

bool Intersection::test(const Line& a, const Vec2& b) {
	return a.containsPoint(b, true);
}

Option<Vec2> Intersection::get(const Ray& a, const Line& b) {
	const auto dt = a.m_direction.x * b.m_direction.y - b.m_direction.x * a.m_direction.y;
	if (aeq(dt, 0.0f))
		return nullptr;
	const auto dx = a.m_point.x - b.m_pointA.x;
	const auto dy = a.m_point.y - b.m_pointA.y;
	auto u = (a.m_direction.x * dy - a.m_direction.y * dx) / dt;
	auto t = (b.m_direction.x * dy - b.m_direction.y * dx) / dt;

	auto pt = a.sampleAlongRay(t);
	if (aeq(pt, a.m_point)) {
		pt = a.m_point;
		t = 0;
	}
	if (aeq(pt, b.m_pointA)) {
		pt = b.m_pointA;
		u = 0;
	}
	assert(aeq(pt, b.m_pointA + b.m_direction * u));
	return t < 0 ? nullptr : (Option<Vec2>)pt;
}

Option<Vec2> Intersection::get(const Ray& a, const LineSegment& b) {
	auto bdir = b.pointB() - b.pointA();
	auto blen = bdir.length();
	bdir /= blen;

	const auto dt = a.m_direction.x * bdir.y - bdir.x * a.m_direction.y;
	if (aeq(dt, 0.0f))
		return nullptr;
	const auto dx = a.m_point.x - b.pointA().x;
	const auto dy = a.m_point.y - b.pointA().y;
	auto u = (a.m_direction.x * dy - a.m_direction.y * dx) / dt;
	auto t = (bdir.x * dy - bdir.y * dx) / dt;

	auto pt = a.sampleAlongRay(t);
	if (aeq(pt, a.m_point)) {
		pt = a.m_point;
		t = 0;
	}
	if (aeq(pt, b.pointA())) {
		pt = b.pointA();
		u = 0;
	}
	if (aeq(pt, b.pointB())) {
		pt = b.pointB();
		u = blen;
	}
	assert(aeq(pt, b.pointA() + bdir * u));
	return t < 0 || u < 0 || u > blen ? nullptr : (Option<Vec2>)pt;
}

Option<Vec2> Intersection::get(const LineSegment& a, const LineSegment& b) {
	auto Intersection = get(a.containingLine(), b.containingLine());
	if (!Intersection)
		return nullptr;
	if (aeq(Intersection.value(), a.pointA()))
		Intersection = a.pointA();
	if (aeq(Intersection.value(), a.pointB()))
		Intersection = a.pointB();
	if (aeq(Intersection.value(), b.pointA()))
		Intersection = b.pointA();
	if (aeq(Intersection.value(), b.pointB()))
		Intersection = b.pointB();

	auto dirA = a.pointB() - a.pointA();
	const auto dirALen = dirA.length();
	dirA /= dirALen;
	auto dirB = b.pointB() - b.pointA();
	const auto dirBLen = dirB.length();
	dirB /= dirBLen;
	const auto t = dot(Intersection.value() - a.pointA(), dirA);
	const auto u = dot(Intersection.value() - b.pointA(), dirB);
	if (t >= 0 && u >= 0 && t <= dirALen && u <= dirBLen)
		return Intersection;
	else
		return nullptr;
}

bool Intersection::test(const LineSegment& a, const Vec2& b) {
	return a.containsPoint(b, true);
}

Option<Vec2> Intersection::get(const Ray& a, const Rect& b) {
	Option<Vec2> pt = nullptr;
	Option<bool> inside = nullptr;

	float t = std::numeric_limits<float>::max();
	for (size_t i = 0; i < 4; i++) {
		const auto ed = b.edge(i);
		auto r = Intersection::get(a, ed);
		if (!r)
			continue;
		if (aeq(r.value(), ed.pointA())) {
			if (!inside.hasValue())
				inside = test(b, a.m_point);

			if (!inside.value() && !(-a.direction()).isBetween(b.normal(i), b.normal(i - 1)))
				continue;
		}
		else if (aeq(r.value(), ed.pointB())) {
			if (!inside.hasValue())
				inside = test(b, a.m_point);

			if (!inside.value() && !(-a.direction()).isBetween(b.normal(i), b.normal(i + 1)))
				continue;
		}
		auto u = a.computeT(r.value());
		if (u < t) {
			t = u;
			pt = r;
		}
	}
	return pt;
}

IntersectionInfo Intersection::get(const Rect& a, const Rect& b) {
	float t = std::numeric_limits<float>::max();
	Vec2 normal;
	for (size_t i = 0; i < 4; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (size_t j = 0; j < 4; j++) {
			auto v = b.point(j) - a.edge(i).pointA();
			auto dt = dot(a.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = a.edge(i).containingLine().squaredDistanceFrom(b.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = -a.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	for (size_t i = 0; i < 4; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (size_t j = 0; j < 4; j++) {
			auto v = a.point(j) - b.edge(i).pointA();
			auto dt = dot(b.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = b.edge(i).containingLine().squaredDistanceFrom(a.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = b.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	return IntersectionInfo(normal, sqrtf(t));
}

Option<Vec2> Intersection::get(const Ray& a, const Circle& b) {
	const auto f = a.point() - b.center();
	float _a = dot(a.direction(), a.direction());
	float _b = 2 * dot(f, a.direction());
	float _c = dot(f, f) - b.radius() * b.radius();
	float d = _b * _b - 4 * _a * _c;
	if (d <= 0) //if d == 0, this means that the ray is tangent to the circle... we don't wanna this Intersection...
		return nullptr;
	else {
		d = sqrtf(d);
		float t1 = (-_b - d) / (2 * _a);
		float t2 = (-_b + d) / (2 * _a);
		if (t1 >= 0) {
			if (t2 >= 0) {
				if (t1 < t2)
					return a.sampleAlongRay(t1);
				else
					return a.sampleAlongRay(t2);
			}
			else {
				return a.sampleAlongRay(t1);
			}
		}
		else {
			if (t2 >= 0)
				return a.sampleAlongRay(t2);
			else
				return nullptr;
		}
	}
}

IntersectionInfo Intersection::get(const Circle& a, const Circle& b) {
	auto v = a.center() - b.center();
	auto vlen = v.length();
	auto maxlen = a.radius() + b.radius();
	if (vlen > maxlen)
		return IntersectionInfo();
	else {
		v /= vlen;
		return IntersectionInfo(v, maxlen - vlen);
	}
}

bool Intersection::test(const Rect& a, const Vec2& b) {
	return a.containsPoint(b);
}

bool Intersection::test(const Rect& a, const Rect& b) {
	const auto nleft = std::max(a.left(), b.left());
	const auto nbottom = std::max(a.bottom(), b.bottom());
	const auto xoverlap = std::min(a.right(), b.right()) - nleft;
	if (xoverlap < 0)
		return false;
	const auto yoverlap = std::min(a.top(), b.top()) - nbottom;
	if (yoverlap < 0)
		return false;
	return true;
}

bool Intersection::test(const Circle& a, const Circle& b) {
	float d = distance(a.center(), b.center());
	float maxlen = a.radius() + b.radius();
	return d <= maxlen;
}

bool Intersection::test(const Circle& a, const Vec2& b) {
	return a.containsPoint(b);
}

IntersectionInfo Intersection::get(const Rect& a, const Circle& b) {
	float t = std::numeric_limits<float>::max();
	Vec2 normal;
	for (size_t i = 0; i < 4; i++) {
		auto v = b.center() - a.edge(i).pointA();
		auto dt1 = dot(a.normal(i), v);
		auto dt2 = a.edge(i).containingLine().distanceFrom(b.center());
		if (dt1 <= 0 || dt2 <= b.radius()) {
			if (dt1 >= 0) {
				auto currentT = b.radius() - dt2;
				if (currentT < t) {
					t = currentT * currentT;
					normal = -a.normal(i);
				}
			}
			else {
				auto currentT = b.radius() + dt2;
				if (currentT < t) {
					t = currentT * currentT;
					normal = -a.normal(i);
				}
			}
		}
		else
			return IntersectionInfo(); //found separating axis

		auto n = a.point(i) - b.center();
		n.normalize();
		auto pt = b.center() + n * b.radius();
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (size_t j = 0; j < 4; j++) {
			auto v = a.point(j) - pt;
			auto dt = dot(n, v);
			if (dt <= 0) {
				foundNegative = true;
				auto ln = Line(pt, pt + n.rotated90());
				auto currentT = ln.squaredDistanceFrom(a.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = n;
				}
			}
		}

		if (!foundNegative) //found separating axis
			return IntersectionInfo();
		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	return IntersectionInfo(normal, sqrtf(t));
}

Option<Vec2> sb::Intersection::get(const Ray& a, const Polygon& b) {
	Option<Vec2> pt = nullptr;
	Option<bool> inside = nullptr;
	float t = std::numeric_limits<float>::max();
	auto eCount = b.edgeCount();
	for (ptrdiff_t i = 0; i < eCount; i++) {
		const auto ed = b.edge(i);
		auto r = Intersection::get(a, ed);
		if (!r)
			continue;
		if (aeq(r.value(), ed.pointA())) {
			if (!inside.hasValue())
				inside = test(b, a.m_point);

			if (!inside.value() && !(-a.direction()).isBetween(b.normal(i), b.normal(i - 1)))
				continue;
		}
		else if (aeq(r.value(), ed.pointB())) {
			if (!inside.hasValue())
				inside = test(b, a.m_point);

			if (!inside.value() && !(-a.direction()).isBetween(b.normal(i), b.normal(i + 1)))
				continue;
		}
		auto u = a.computeT(r.value());
		if (u < t) {
			t = u;
			pt = r;
		}
	}
	return pt;
}

sb::IntersectionInfo sb::Intersection::get(const Polygon& a, const Polygon& b) {
	assert(a.isSimple());
	assert(b.isSimple());
	assert(a.isConvex());
	assert(b.isConvex());
	assert(a.area() != 0);
	assert(b.area() != 0);
	float t = std::numeric_limits<float>::max();
	Vec2 normal;
	auto aeCount = a.edgeCount();
	auto beCount = b.edgeCount();
	for (ptrdiff_t i = 0; i < aeCount; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (ptrdiff_t j = 0; j < b.pointCount(); j++) {
			auto v = b.point(j) - a.edge(i).pointA();
			auto dt = dot(a.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = a.edge(i).containingLine().squaredDistanceFrom(b.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = -a.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	for (ptrdiff_t i = 0; i < beCount; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (ptrdiff_t j = 0; j < a.pointCount(); j++) {
			auto v = a.point(j) - b.edge(i).pointA();
			auto dt = dot(b.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = b.edge(i).containingLine().squaredDistanceFrom(a.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = b.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	return IntersectionInfo(normal, sqrtf(t));
}

sb::IntersectionInfo sb::Intersection::get(const Polygon& a, const Rect& b) {
	assert(a.isSimple());
	assert(a.isConvex());
	assert(a.area() != 0);
	float t = std::numeric_limits<float>::max();
	Vec2 normal;
	auto aeCount = a.edgeCount();
	for (ptrdiff_t i = 0; i < aeCount; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (size_t j = 0; j < 4; j++) {
			auto v = b.point(j) - a.edge(i).pointA();
			auto dt = dot(a.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = a.edge(i).containingLine().squaredDistanceFrom(b.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = -a.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	for (size_t i = 0; i < 4; i++) {
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (ptrdiff_t j = 0; j < a.pointCount(); j++) {
			auto v = a.point(j) - b.edge(i).pointA();
			auto dt = dot(b.normal(i), v);
			if (dt <= 0) {
				foundNegative = true;
				auto currentT = b.edge(i).containingLine().squaredDistanceFrom(a.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = b.normal(i);
				}
			}
		}

		if (!foundNegative)
			return IntersectionInfo();

		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	return IntersectionInfo(normal, sqrtf(t));
}

sb::IntersectionInfo sb::Intersection::get(const Polygon& a, const Circle& b) {
	assert(a.isSimple());
	assert(a.isConvex());
	assert(a.area() != 0);
	float t = std::numeric_limits<float>::max();
	auto aeCount = a.edgeCount();
	Vec2 normal;
	for (ptrdiff_t i = 0; i < aeCount; i++) {
		auto v = b.center() - a.edge(i).pointA();
		auto dt1 = dot(a.normal(i), v);
		auto dt2 = a.edge(i).containingLine().distanceFrom(b.center());
		if (dt1 <= 0 || dt2 <= b.radius()) {
			if (dt1 >= 0) {
				auto currentT = b.radius() - dt2;
				if (currentT < t) {
					t = currentT * currentT;
					normal = -a.normal(i);
				}
			}
			else {
				auto currentT = b.radius() + dt2;
				if (currentT < t) {
					t = currentT * currentT;
					normal = -a.normal(i);
				}
			}
		}
		else
			return IntersectionInfo(); //found separating axis

		auto n = a.point(i) - b.center();
		n.normalize();
		auto pt = b.center() + n * b.radius();
		bool foundNegative = false;
		float tEdge = std::numeric_limits<float>::lowest();
		Vec2 eNormal;
		for (ptrdiff_t j = 0; j < a.pointCount(); j++) {
			auto v = a.point(j) - pt;
			auto dt = dot(n, v);
			if (dt <= 0) {
				foundNegative = true;
				auto ln = Line(pt, pt + n.rotated90());
				auto currentT = ln.squaredDistanceFrom(a.point(j));
				if (currentT > tEdge) {
					tEdge = currentT;
					eNormal = n;
				}
			}
		}

		if (!foundNegative) //found separating axis
			return IntersectionInfo();
		if (tEdge < t) {
			t = tEdge;
			normal = eNormal;
		}
	}

	return IntersectionInfo(normal, sqrtf(t));
}

bool sb::Intersection::test(const Polygon& a, const Vec2& b) {
	return a.containsPoint(b);
}
