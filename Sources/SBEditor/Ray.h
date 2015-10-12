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
	struct Ray {
	public:
		//Constructors
		inline Ray() {
			m_point = Vec2::zero;
			m_direction = Vec2::zero;
		}
		inline Ray(const Vec2& point, const Vec2& direction, bool normalize = true) {
			m_point = point;
			m_direction = direction;
			if (normalize)
				m_direction.normalize();
		}
		//Accessors
		inline const Vec2& point() const {
			return m_point;
		}
		inline void setPoint(const Vec2& value) {
			m_point = value;
		}
		inline const Vec2& direction() const {
			return m_direction;
		}
		inline void setDirection(const Vec2& value, bool normalize = true) {
			m_direction = value;
			if (normalize)
				m_direction.normalize();
		}
		//Tests
		inline bool isParallelTo(const Ray& another, bool almost = false) const {
			return m_direction.isParallelTo(another.m_direction, almost);
		}
		inline bool isPerpendicularTo(const Ray& another, bool almost = false) const {
			return m_direction.isPerpendicularTo(another.m_direction, almost);
		}
		inline bool containsPoint(const Vec2& point, bool almost = false) const {
			auto v = point - m_point;
			auto vx = v.projectionOver(m_direction);
			auto vy = v - vx;
			return vy.isZero(almost) && dot(vx, m_direction) >= 0;
		}
		inline bool isCollinearTo(const Ray& another, bool almost = false) const {
			//Line contains point?
			auto v = another.m_point - m_point;
			auto p = v - v.projectionOver(m_direction);
			auto r = p.isZero(almost);

			return isParallelTo(another, almost) && r;
		}
		//Operations with points
		inline Vec2 sampleAlongRay(float t) const {
			return m_point + m_direction * t;
		}
		inline float computeT(const Vec2& point) const {
			auto v = point - m_point;
			v.projectOver(m_direction);
			return dot(v, m_direction) >= 0 ? v.length() : -v.length();
		}
		inline Vec2 perpendicularVector(const Vec2& point) const {
			auto v = point - m_point;
			return v - v.projectionOver(m_direction);
		}
		inline Vec2 distanceVector(const Vec2& point) const {
			auto v = point - m_point;
			auto vx = v.projectionOver(m_direction);
			auto vy = v - vx;
			return dot(vx, m_direction) >= 0 ? vy : v;
		}
		inline float distanceFrom(const Vec2& point) const {
			return distanceVector(point).length();
		}
		inline float squaredDistanceFrom(const Vec2& point) const {
			return distanceVector(point).squaredLength();
		}
		//Description
		inline std::string toString() const {
			return "{" + m_point.toString() + ", " + m_direction.toString() + "}";
		}
		//Friends
		friend bool operator ==(const Ray&, const Ray&);
		friend bool operator !=(const Ray&, const Ray&);
		friend bool operator >(const Ray&, const Ray&);
		friend bool operator <(const Ray&, const Ray&);
		friend bool operator >=(const Ray&, const Ray&);
		friend bool operator <=(const Ray&, const Ray&);
		friend bool aeq(const Ray&, const Ray&);
		//Friend classes
		friend class Intersection;
	private:
		Vec2 m_point;
		Vec2 m_direction;
	};
	inline bool operator ==(const Ray& r1, const Ray& r2) {
		return r1.m_point == r2.m_point && r1.m_direction == r2.m_direction;
	}
	inline bool operator !=(const Ray& r1, const Ray& r2) {
		return r1.m_point != r2.m_point || r1.m_direction != r2.m_direction;
	}
	inline bool operator >(const Ray& r1, const Ray& r2) {
		if (r1.m_point == r2.m_point)
			return r1.m_direction > r2.m_direction;
		else
			return r1.m_point > r2.m_point;
	}
	inline bool operator <(const Ray& r1, const Ray& r2) {
		if (r1.m_point == r2.m_point)
			return r1.m_direction < r2.m_direction;
		else
			return r1.m_point < r2.m_point;
	}
	inline bool operator >=(const Ray& r1, const Ray& r2) {
		if (r1 == r2)
			return true;

		if (r1.m_point == r2.m_point)
			return r1.m_direction > r2.m_direction;
		else
			return r1.m_point > r2.m_point;
	}
	inline bool operator <=(const Ray& r1, const Ray& r2) {
		if (r1 == r2)
			return true;

		if (r1.m_point == r2.m_point)
			return r1.m_direction < r2.m_direction;
		else
			return r1.m_point < r2.m_point;
	}
	inline bool aeq(const Ray& r1, const Ray& r2) {
		return aeq(r1.m_point, r2.m_point) && aeq(r1.m_direction, r2.m_direction);
	}
}

namespace std {
	template<>
	class hash < sb::Ray > {
	public:
		size_t operator()(const sb::Ray& v) const {
			return sb::bitwiseHash((const unsigned char*)&v, sizeof(sb::Ray));
		}
	};
}

