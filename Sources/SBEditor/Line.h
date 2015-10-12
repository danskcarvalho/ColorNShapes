/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

#include "Utils.h"
#include "Vec2.h"

namespace sb {
	//Forward Declarations
	class Intersection;
	//Class
	struct Line {
	public:
		//Constructors
		inline Line() {
			m_pointA = Vec2::zero;
			m_pointB = Vec2::zero;
			m_direction = Vec2::zero;
		}
		Line(const Vec2& pointA, const Vec2& pointB) {
			m_pointA = pointA;
			m_pointB = pointB;
			m_direction = m_pointB - m_pointA;
			m_direction.normalize();
		}
		//Accessors
		inline const Vec2& pointA() const {
			return m_pointA;
		}
		inline const Vec2& pointB() const {
			return m_pointB;
		}
		inline void setPointA(const Vec2& value) {
			m_pointA = value;
			m_direction = m_pointB - m_pointA;
			m_direction.normalize();
		}
		inline void setPointB(const Vec2& value) {
			m_pointB = value;
			m_direction = m_pointB - m_pointA;
			m_direction.normalize();
		}
		inline void setPoints(const Vec2& pointA, const Vec2& pointB) {
			m_pointA = pointA;
			m_pointB = pointB;
			m_direction = m_pointB - m_pointA;
			m_direction.normalize();
		}
		//Tests
		inline bool isParallelTo(const Line& another, bool almost = false) const {
			return m_direction.isParallelTo(another.m_direction, almost);
		}
		inline bool isPerpendicularTo(const Line& another, bool almost = false) const {
			return m_direction.isPerpendicularTo(another.m_direction, almost);
		}
		inline bool containsPoint(const Vec2& point, bool almost = false) const {
			auto v = point - m_pointA;
			auto p = v - v.projectionOver(m_direction);
			return p.isZero(almost);
		}
		inline bool isCollinearTo(const Line& another, bool almost = false) const {
			return isParallelTo(another, almost) && containsPoint(another.m_pointA);
		}
		//Point Operations
		inline Vec2 perpendicularVector(const Vec2& point) const {
			auto v = point - m_pointA;
			return v - v.projectionOver(m_direction);
		}
		inline float distanceFrom(const Vec2& point) const {
			return perpendicularVector(point).length();
		}
		inline float squaredDistanceFrom(const Vec2& point) const {
			return perpendicularVector(point).squaredLength();
		}
		inline Vec2 closestPointFrom(const Vec2& point) const {
			return point - perpendicularVector(point);
		}
		//Description
		inline std::string toString() const {
			return "{" + m_pointA.toString() + ", " + m_pointB.toString() + "}";
		}
		//Friends
		friend bool operator ==(const Line&, const Line&);
		friend bool operator !=(const Line&, const Line&);
		friend bool aeq(const Line&, const Line&);
		//Friend classes
		friend class Intersection;
	private:
		//Members
		Vec2 m_pointA;
		Vec2 m_pointB;
		Vec2 m_direction;
	};
	inline bool operator ==(const Line& l1, const Line& l2) {
		return l1.isCollinearTo(l2);
	}
	inline bool operator !=(const Line& l1, const Line& l2) {
		return !l1.isCollinearTo(l2);
	}
	inline bool aeq(const Line& l1, const Line& l2) {
		return l1.isCollinearTo(l2, true);
	}
}

