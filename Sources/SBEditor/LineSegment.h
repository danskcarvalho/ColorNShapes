/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Utils.h"
#include "Vec2.h"
#include "Line.h"

namespace sb {
	struct LineSegment {
	public:
		inline LineSegment() {
			m_pointA = Vec2::zero;
			m_pointB = Vec2::zero;
		}
		inline LineSegment(const Vec2& pointA, const Vec2& pointB) {
			m_pointA = pointA;
			m_pointB = pointB;
		}
		//Accessors
		inline const Vec2& pointA() const {
			return m_pointA;
		}
		inline void setPointA(const Vec2& value) {
			m_pointA = value;
		}
		inline const Vec2& pointB() const {
			return m_pointB;
		}
		inline void setPointB(const Vec2& value) {
			m_pointB = value;
		}
		inline Line containingLine() const {
			return Line(m_pointA, m_pointB);
		}
		//Info
		inline float length() {
			return distance(m_pointA, m_pointB);
		}
		inline bool isDegenerated(bool almost = false) const {
			return almost ? aeq(m_pointA, m_pointB) : m_pointA == m_pointB;
		}
		inline LineSegment toCanonical() const {
			return m_pointA >= m_pointB ? *this : LineSegment(m_pointB, m_pointA);
		}
		//Operations with pointers
		inline bool containsPoint(const Vec2& point, bool almost = false) const {
			auto v = point - m_pointA;
			auto direction = m_pointB - m_pointA;
			auto segmentLength = direction.length();
			direction /= segmentLength;
			auto vx = v.projectionOver(direction);
			auto vy = v - vx;
			auto dt = dot(vx, direction);
			return vy.isZero(almost) && dt >= 0 && dt <= segmentLength;
		}
		inline Vec2 distanceVector(const Vec2& point) const {
			auto v = point - m_pointA;
			auto direction = m_pointB - m_pointA;
			auto segmentLength = direction.length();
			direction /= segmentLength;
			auto vx = v.projectionOver(direction);
			auto vy = v - vx;
			auto dt = dot(vx, direction);
			if (dt >= 0 && dt <= segmentLength)
				return vy;
			else {
				if (dt < 0)
					return v;
				else
					return point - m_pointB;
			}
		}
		inline float distanceFrom(const Vec2& point) const {
			return distanceVector(point).length();
		}
		inline float squaredDistanceFrom(const Vec2& point) const {
			return distanceVector(point).squaredLength();
		}
		inline Vec2 closestPointFrom(const Vec2& point) const {
			return point - distanceVector(point);
		}
		//Description
		inline std::string toString() const {
			return "{" + m_pointA.toString() + ", " + m_pointB.toString() + "}";
		}
		//Friends
		friend bool operator ==(const LineSegment&, const LineSegment&);
		friend bool operator !=(const LineSegment&, const LineSegment&);
		friend bool operator >(const LineSegment&, const LineSegment&);
		friend bool operator <(const LineSegment&, const LineSegment&);
		friend bool operator >=(const LineSegment&, const LineSegment&);
		friend bool operator <=(const LineSegment&, const LineSegment&);
		friend bool aeq(const LineSegment&, const LineSegment&);
	private:
		Vec2 m_pointA;
		Vec2 m_pointB;
	};

	inline bool operator ==(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		return c1.m_pointA == c2.m_pointA && c1.m_pointB == c2.m_pointB;
	}
	inline bool operator !=(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		return c1.m_pointA != c2.m_pointA || c1.m_pointB != c2.m_pointB;
	}
	inline bool operator >(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		if (c1.m_pointA == c2.m_pointA)
			return c1.m_pointB > c2.m_pointB;
		else
			return c1.m_pointA > c2.m_pointA;
	}
	inline bool operator <(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		if (c1.m_pointA == c2.m_pointA)
			return c1.m_pointB < c2.m_pointB;
		else
			return c1.m_pointA < c2.m_pointA;
	}
	inline bool operator >=(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		if (c1.m_pointA == c2.m_pointA && c1.m_pointB == c2.m_pointB)
			return true;
		if (c1.m_pointA == c2.m_pointA)
			return c1.m_pointB > c2.m_pointB;
		else
			return c1.m_pointA > c2.m_pointA;
	}
	inline bool operator <=(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		if (c1.m_pointA == c2.m_pointA && c1.m_pointB == c2.m_pointB)
			return true;
		if (c1.m_pointA == c2.m_pointA)
			return c1.m_pointB < c2.m_pointB;
		else
			return c1.m_pointA < c2.m_pointA;
	}
	inline bool aeq(const LineSegment& s1, const LineSegment& s2) {
		const auto c1 = s1.toCanonical();
		const auto c2 = s2.toCanonical();
		return aeq(c1.m_pointA, c2.m_pointA) && aeq(c1.m_pointB, c2.m_pointB);
	}
}

namespace std {
	template<>
	class hash < sb::LineSegment > {
	public:
		size_t operator()(const sb::LineSegment& v) const {
			return sb::bitwiseHash((const unsigned char*)&v, sizeof(sb::LineSegment));
		}
	};
}

