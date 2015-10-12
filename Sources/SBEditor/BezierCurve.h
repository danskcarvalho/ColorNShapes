/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/


#pragma once

#include "Vec2.h"
#include "Line.h"

namespace sb {
	struct Polygon;
	struct BezierCurve {
	public:
		//Members
		Vec2 point0;
		Vec2 point1;
		Vec2 point2;
		Vec2 point3;
		//Constructors
		inline BezierCurve() {
			point0 = point1 = point2 = point3 = Vec2::zero;
		}
		inline BezierCurve(const Vec2& point0, const Vec2& point1, const Vec2& point2, const Vec2& point3) {
			this->point0 = point0;
			this->point1 = point1;
			this->point2 = point2;
			this->point3 = point3;
		}
		//Operations
		inline Vec2 sampleAlongCurve(float u) const {
			const auto t1 = (1 - u);
			const auto t2 = t1 * t1;
			const auto t3 = t2 * t1;
			const auto u2 = u * u;
			const auto u3 = u2 * u;
			return t3 * point0 + 3 * u * t2 * point1 + 3 * u2 * t1 * point2 + u3 * point3;
		}
		inline void subdivide(BezierCurve* b1, BezierCurve* b2) const {
			const auto L0 = point0;
			const auto R3 = point3;
			const auto L1 = (point0 + point1) * 0.5f;
			const auto H = (point1 + point2) * 0.5f;
			const auto R2 = (point2 + point3) * 0.5f;
			const auto L2 = (L1 + H) * 0.5f;
			const auto R1 = (H + R2) * 0.5f;
			const auto L3 = (L2 + R1) * 0.5f;
			const auto R0 = L3;
			b1->point0 = L0;
			b1->point1 = L1;
			b1->point2 = L2;
			b1->point3 = L3;
			b2->point0 = R0;
			b2->point1 = R1;
			b2->point2 = R2;
			b2->point3 = R3;
		}
		//Tests
		inline bool isStraight(float epsilon = 0.0001f) const {
			assert(epsilon > 0);
			const auto l = Line(point0, point3);
			const auto d1 = l.distanceFrom(point1);
			const auto d2 = l.distanceFrom(point2);
			return d1 <= epsilon && d2 <= epsilon;
		}
		//Description
		std::string toString() const {
			return "{" + point0.toString() + ", " + point1.toString() + ", " + point2.toString() +
				", " + point3.toString() + "}";
		}
		//Conversions
		Polygon toPolygon(size_t quality = 1000) const;
		static Polygon toPolygon(const std::vector<BezierCurve>& curves, size_t quality = 1000);
		static Polygon toPolygon(const BezierCurve* curves, size_t count, size_t quality = 1000);
	};

	inline bool operator==(const BezierCurve& b1, const BezierCurve& b2) {
		return b1.point0 == b2.point0 && b1.point1 == b2.point1 && b1.point2 == b2.point2 &&
			b1.point3 == b2.point3;
	}
	inline bool operator!=(const BezierCurve& b1, const BezierCurve& b2) {
		return b1.point0 != b2.point0 || b1.point1 != b2.point1 || b1.point2 != b2.point2 ||
			b1.point3 != b2.point3;
	}
	inline bool aeq(const BezierCurve& b1, const BezierCurve& b2) {
		return aeq(b1.point0, b2.point0) && aeq(b1.point1, b2.point1) && aeq(b1.point2, b2.point2) &&
			aeq(b1.point3, b2.point3);
	}
	inline bool operator>(const BezierCurve& b1, const BezierCurve& b2) {
		if (b1.point0 == b2.point0) {
			if (b1.point1 == b2.point1) {
				if (b1.point2 == b2.point2)
					return b1.point3 > b2.point3;
				else
					return b1.point2 > b2.point2;
			}
			else
				return b1.point1 > b2.point1;
		}
		else
			return b1.point0 > b2.point0;
	}
	inline bool operator<(const BezierCurve& b1, const BezierCurve& b2) {
		if (b1.point0 == b2.point0) {
			if (b1.point1 == b2.point1) {
				if (b1.point2 == b2.point2)
					return b1.point3 < b2.point3;
				else
					return b1.point2 < b2.point2;
			}
			else
				return b1.point1 < b2.point1;
		}
		else
			return b1.point0 < b2.point0;
	}
	inline bool operator>=(const BezierCurve& b1, const BezierCurve& b2) {
		if (b1 == b2)
			return true;
		if (b1.point0 == b2.point0) {
			if (b1.point1 == b2.point1) {
				if (b1.point2 == b2.point2)
					return b1.point3 > b2.point3;
				else
					return b1.point2 > b2.point2;
			}
			else
				return b1.point1 > b2.point1;
		}
		else
			return b1.point0 > b2.point0;
	}
	inline bool operator<=(const BezierCurve& b1, const BezierCurve& b2) {
		if (b1 == b2)
			return true;

		if (b1.point0 == b2.point0) {
			if (b1.point1 == b2.point1) {
				if (b1.point2 == b2.point2)
					return b1.point3 < b2.point3;
				else
					return b1.point2 < b2.point2;
			}
			else
				return b1.point1 < b2.point1;
		}
		else
			return b1.point0 < b2.point0;
	}
}

namespace std {
	template<>
	class hash < sb::BezierCurve > {
	public:
		size_t operator()(const sb::BezierCurve& b) const {
			return sb::bitwiseHash((const unsigned char*)&b, sizeof(sb::BezierCurve));
		}
	};
}

