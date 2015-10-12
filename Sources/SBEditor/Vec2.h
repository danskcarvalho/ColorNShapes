/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

#include "Utils.h"

namespace sb {
	enum class RotationDirection {
		shortest,
		ccw,
		cw
	};
	struct Vec2 {
	public:
		//Members
		float x, y;
		//Static Members
		static const Vec2 up;
		static const Vec2 down;
		static const Vec2 left;
		static const Vec2 right;
		static const Vec2 zero;
		static const Vec2 one;
		//Constructors
		inline Vec2() {
			x = 0;
			y = 0;
		}
		inline Vec2(float x, float y) {
			this->x = x;
			this->y = y;
		}
		//Basic Functions
		//Length
		inline float length() const {
			return sqrt(x * x + y * y);
		}
		inline float squaredLength() const {
			return x * x + y * y;
		}
		inline Vec2 normalized() const {
			auto len = length();
			return Vec2(x / len, y / len);
		}
		inline void normalize() {
			auto len = length();
			x /= len;
			y /= len;
		}
		//Size Functions
		inline float area() const {
			return x * y;
		}
		inline float width() const {
			return x;
		}
		inline float height() const {
			return y;
		}
		inline void setWidth(float value) {
			x = value;
		}
		inline void setHeight(float value) {
			y = value;
		}
		//Tests
		inline bool isZero(bool almost = false) const {
			return almost ? (aeq(x, 0) && aeq(y, 0)) : (x == 0 && y == 0);
		}
		inline bool isUnit(bool almost = false) const {
			auto len = squaredLength();
			return almost ? aeq(len, 1) : len == 1;
		}
		inline bool isPerpendicularTo(const Vec2& another, bool almost = false) const {
			auto dt = x * another.x + y * another.y;
			return almost ? aeq(dt, 0) : dt == 0;
		}
		inline bool isParallelTo(const Vec2& another, bool almost = false) const {
			auto dt = x * another.x + y * another.y;
			auto shouldBeZero = dt * dt - this->squaredLength() * another.squaredLength();
			return almost ? aeq(shouldBeZero, 0) : shouldBeZero == 0;
		}
		inline bool pointsAtSameDirection(const Vec2& another) const {
			auto dt = x * another.x + y * another.y;
			return dt >= 0;
		}
		//Angle
		inline float angleBetween(Vec2 another, RotationDirection rd = RotationDirection::shortest) const {
			if (this->isZero() || another.isZero())
				return 0;

			auto dt = x * another.x + y * another.y;
			dt /= length() * another.length();

			if (rd == RotationDirection::shortest)
				return acosf(dt);
			else {
				auto c = x * another.y - another.x * y;
				if (c == 0) //it may be 0 or +PI
				{
					if (this->pointsAtSameDirection(another)) {
						return 0;
					}
					else {
						return (float)SbPI;
					}
				}

				if (rd == RotationDirection::ccw) {
					if (c > 0) {
						return acosf(dt);
					}
					else {
						return 2 * (float)SbPI - acosf(dt);
					}
				}
				else {
					if (c > 0) {
						return 2 * (float)SbPI - acosf(dt);
					}
					else {
						return acosf(dt);
					}
				}
			}
		}
		inline bool isBetween(const Vec2& v1, const Vec2& v2) const {
			auto c1 = sign(v1.x * v2.y - v2.x * v1.y);
			auto c2 = sign(v1.x * y - x * v1.y);
			auto c3 = sign(x * v2.y - v2.x * y);
			return c1 == c2 && c2 == c3;
		}
		//Projection
		inline void projectOver(const Vec2& v) {
			auto nv = v.normalized();
			auto dt = x * nv.x + y * nv.y;
			x = nv.x * dt;
			y = nv.y * dt;

		}
		inline Vec2 projectionOver(const Vec2& v) const {
			auto nv = v.normalized();
			auto dt = x * nv.x + y * nv.y;
			return Vec2(nv.x * dt, nv.y * dt);

		}
		//Rotation
		inline void rotate90() {
			auto sx = x;
			x = -y;
			y = sx;
		}
		inline Vec2 rotated90() const {
			auto v = *this;
			v.rotate90();
			return v;
		}
		inline void rotateBy(float angle) {
			auto v = *this;
			auto cos0 = cosf(angle);
			auto sin0 = sinf(angle);
			v.x = x * cos0 - y * sin0;
			v.y = x * sin0 + y * cos0;
			*this = v;
		}
		inline Vec2 rotatedBy(float angle) const {
			auto v = *this;
			auto cos0 = cosf(angle);
			auto sin0 = sinf(angle);
			v.x = x * cos0 - y * sin0;
			v.y = x * sin0 + y * cos0;
			return v;
		}
		//Description
		inline std::string toString() const {
			return "{" + std::to_string(x) + " , " + std::to_string(y) + "}";
		}
		//Operators
		inline Vec2& operator+=(const Vec2& v2) {
			this->x += v2.x;
			this->y += v2.y;
			return *this;
		}
		inline Vec2& operator*=(const Vec2& v2) {
			this->x *= v2.x;
			this->y *= v2.y;
			return *this;
		}
		inline Vec2& operator-=(const Vec2& v2) {
			this->x -= v2.x;
			this->y -= v2.y;
			return *this;
		}
		inline Vec2& operator*=(float s) {
			this->x *= s;
			this->y *= s;
			return *this;
		}
		inline Vec2& operator/=(const Vec2& v2) {
			this->x /= v2.x;
			this->y /= v2.y;
			return *this;
		}
		inline Vec2& operator/=(float s) {
			this->x /= s;
			this->y /= s;
			return *this;
		}
	};
	inline Vec2 operator +(const Vec2& v1) {
		return v1;
	}
	inline Vec2 operator -(const Vec2& v1) {
		return Vec2(-v1.x, -v1.y);
	}
	inline Vec2 operator+(const Vec2& v1, const Vec2& v2) {
		return Vec2(v1.x + v2.x, v1.y + v2.y);
	}
	inline Vec2 operator-(const Vec2& v1, const Vec2& v2) {
		return Vec2(v1.x - v2.x, v1.y - v2.y);
	}
	inline Vec2 operator*(const Vec2& v1, const Vec2& v2) {
		return Vec2(v1.x * v2.x, v1.y * v2.y);
	}
	inline Vec2 operator*(const Vec2& v1, float s) {
		return Vec2(v1.x * s, v1.y * s);
	}
	inline Vec2 operator*(float s, const Vec2& v1) {
		return Vec2(v1.x * s, v1.y * s);
	}
	inline Vec2 operator/(const Vec2& v1, const Vec2& v2) {
		return Vec2(v1.x / v2.x, v1.y / v2.y);
	}
	inline Vec2 operator/(const Vec2& v1, float s) {
		return Vec2(v1.x / s, v1.y / s);
	}
	inline bool operator==(const Vec2& v1, const Vec2& v2) {
		return v1.x == v2.x && v1.y == v2.y;
	}
	inline bool operator!=(const Vec2& v1, const Vec2& v2) {
		return v1.x != v2.x || v1.y != v2.y;
	}
	inline bool operator>(const Vec2& v1, const Vec2& v2) {
		if (v1.x == v2.x) {
			return v1.y > v2.y;
		}
		else {
			return v1.x > v2.x;
		}
	}
	inline bool operator<(const Vec2& v1, const Vec2& v2) {
		if (v1.x == v2.x) {
			return v1.y < v2.y;
		}
		else {
			return v1.x < v2.x;
		}
	}
	inline bool operator>=(const Vec2& v1, const Vec2& v2) {
		if (v1.x == v2.x && v1.y == v2.y)
			return true;

		if (v1.x == v2.x)
			return v1.y > v2.y;
		else
			return v1.x > v2.x;
	}
	inline bool operator<=(const Vec2& v1, const Vec2& v2) {
		if (v1.x == v2.x && v1.y == v2.y)
			return true;

		if (v1.x == v2.x)
			return v1.y < v2.y;
		else
			return v1.x < v2.x;
	}
	inline bool aeq(const Vec2& v1, const Vec2& v2) {
		return aeq(v1.x, v2.x) && aeq(v1.y, v2.y);
	}
	inline float dot(const Vec2& v1, const Vec2& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
	inline float distance(const Vec2& v1, const Vec2& v2) {
		auto dx = v1.x - v2.x;
		auto dy = v1.y - v2.y;
		return sqrt(dx * dx + dy * dy);
	}
	inline float cross(const Vec2& v1, const Vec2& v2) {
		return v1.x * v2.y - v2.x * v1.y;
	}
	inline Vec2 lerp(float t, const Vec2& v1, const Vec2& v2) {
		return (1 - t) * v1 + t * v2;
	}
}

namespace std {
	template<>
	class hash < sb::Vec2 > {
	public:
		size_t operator()(const sb::Vec2& v) const {
			return sb::bitwiseHash((const unsigned char*)&v, sizeof(sb::Vec2));
		}
	};
}

