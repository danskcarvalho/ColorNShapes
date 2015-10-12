/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Vec2.h"
#include "Rect.h"

namespace sb {
	struct Polygon;
	struct Circle {
	public:
		//Static
		static Circle unit;
		//Constructors
		inline Circle() {
			m_center = Vec2::zero;
			m_radius = 0;
		}
		inline Circle(const Vec2& center, float radius) {
			assert(radius >= 0);
			m_center = center;
			m_radius = radius;
		}
		//Accessors
		inline const Vec2& center() const {
			return m_center;
		}
		inline void setCenter(const Vec2& value) {
			m_center = value;
		}
		inline float radius() const {
			return m_radius;
		}
		inline void setRadius(float value) {
			assert(value >= 0);
			m_radius = value;
		}
		inline float diameter() const {
			return m_radius * 2;
		}
		inline void setDiameter(float value) {
			assert(value >= 0);
			m_radius = value / 2;
		}
		inline float area() const {
			return SbPI * m_radius * m_radius;
		}
		inline float perimeter() const {
			return 2 * SbPI * m_radius;
		}
		inline Rect bounds() const {
			return Rect(m_center, Vec2(m_radius * 2, m_radius * 2));
		}
		//Tests
		inline bool containsPoint(const Vec2& point) const {
			const auto dt = (point - m_center).squaredLength();
			return dt <= m_radius * m_radius;
		}
		inline bool containsCircle(const Circle& circle) const {
			auto dt = distance(m_center, circle.m_center);
			dt += circle.m_radius;
			return dt <= m_radius;
		}
		//Description
		inline std::string toString() const {
			return "{" + m_center.toString() + ", " + std::to_string(m_radius) + "}";
		}
		//Conversion
		Polygon toPolygon(size_t sides) const;
	private:
		Vec2 m_center;
		float m_radius;
	};

	inline bool operator ==(const Circle& c1, const Circle& c2) {
		return c1.center() == c2.center() && c1.radius() == c2.radius();
	}
	inline bool operator !=(const Circle& c1, const Circle& c2) {
		return c1.center() != c2.center() || c1.radius() != c2.radius();
	}
	inline bool operator >(const Circle& c1, const Circle& c2) {
		if (c1.center() == c2.center())
			return c1.radius() > c2.radius();
		else
			return c1.center() > c2.center();
	}
	inline bool operator <(const Circle& c1, const Circle& c2) {
		if (c1.center() == c2.center())
			return c1.radius() < c2.radius();
		else
			return c1.center() < c2.center();
	}
	inline bool operator >=(const Circle& c1, const Circle& c2) {
		if (c1 == c2)
			return true;

		if (c1.center() == c2.center())
			return c1.radius() > c2.radius();
		else
			return c1.center() > c2.center();
	}
	inline bool operator <=(const Circle& c1, const Circle& c2) {
		if (c1 == c2)
			return true;

		if (c1.center() == c2.center())
			return c1.radius() < c2.radius();
		else
			return c1.center() < c2.center();
	}
	inline bool aeq(const Circle& c1, const Circle& c2) {
		return aeq(c1.center(), c2.center()) && aeq(c1.radius(), c2.radius());
	}
}

namespace std {
	template<>
	class hash < sb::Circle > {
	public:
		size_t operator()(const sb::Circle& c) const {
			return sb::bitwiseHash((const unsigned char*)&c, sizeof(sb::Circle));
		}
	};
}

